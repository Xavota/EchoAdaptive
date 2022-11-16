#pragma warning(push, 0)
#include <d3d11.h>

#if defined (__WIN32__) || defined (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#endif
#pragma warning(pop)

#include <cmath>
#include <vector>
#include <algorithm>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <ImSequencer.h>
#include <ImCurveEdit.h>

#include <fmod.hpp>

#define FMOD_ERROR_CHECK(FMOD_RESULT)                                          \
if (r != FMOD_OK) {                                                            \
  return false;                                                                   \
}

#include "eaSound.h"

#include "eaSoundMixer.h"
#include "eaSoundMixerChannel.h"

#include "eaSoundEvent.h"
#include "eaSoundEventLoop.h"

#include "eaSoundTrack.h"

#include "eaSoundMixerSequencer.h"

using eaSdkEngine::Vector;

using eaSdkEngine::Sound;

using eaSdkEngine::SoundMixer;
using eaSdkEngine::SoundMixerChannel;

using eaSdkEngine::SoundEvent;
using eaSdkEngine::SoundEventLoop;

using eaSdkEngine::SoundTrack;

using eaSdkEngine::SoundMixerSequencer;

SoundMixer gMixer;

#define DEFAULT_PIXELS_PER_SECOND 150

static const ImVec2           gChannelTLOffset = ImVec2(10.0f, 10.0f);
static const ImVec2           gChannelBROffset = ImVec2(10.0f, 10.0f);
static float                  gChannelItemsWidth = 100.0f;

static const Vector<float>    gZoom = { 3.5f, 0.3f, 0.3f, 0.3f };
static int                    gChannIndex = 0;



#define DX11SAFE_RELEASE(x)                                                    \
if(x) {                                                                        \
  x->Release();                                                                \
  x = nullptr;                                                                 \
}                                                                              \

int                         gWindowWidth = 1280;
int                         gWindowHeight = 720;

HWND                        gWindowPtr = nullptr;

ID3D11Device*               gDevice = nullptr;
ID3D11DeviceContext*        gDeviceContext = nullptr;
IDXGISwapChain*             gSwapChain = nullptr;

ID3D11Texture2D*            gTextureResource = nullptr;
ID3D11RenderTargetView*     gBackBufferRTV = nullptr;
ID3D11DepthStencilView*     gDepthStencilView = nullptr;


extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND _hwnd,
                               UINT _msg,
                               WPARAM _wParam,
                               LPARAM _lParam);

void
resizeDX11(int width, int height);

bool firstResize = true;
LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  // Handle UI inputs
  if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
    return 1;
  }

  PAINTSTRUCT ps;
  HDC hdc;

  switch (message) {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_SIZE: {
    if (!firstResize) {
      RECT rc = {};

      GetClientRect(gWindowPtr, &rc);
      int width = rc.right - rc.left;
      int height = rc.bottom - rc.top;

      resizeDX11(width, height);
    }
    firstResize = false;
    break;
  }

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

bool
initWindowDX11(int width, int height)
{
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = reinterpret_cast<WNDPROC>(WndProc);
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = nullptr;
  wcex.hIcon = nullptr;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = "TutorialWindowClass";
  wcex.hIconSm = nullptr;
  if (!RegisterClassEx(&wcex)) {
    return false;
  }

  RECT rc = { 0,
              0,
              static_cast<LONG>(width),
              static_cast<LONG>(height) };

  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  gWindowPtr = CreateWindow("TutorialWindowClass",
                            "Echo Adaptive V. 1.0",
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            rc.right,
                            rc.bottom,
                            nullptr,
                            nullptr,
                            nullptr,
                            nullptr);


  ShowWindow(gWindowPtr, SW_SHOWNORMAL);

  return true;
}

bool
initBasics(int width, int height)
{
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
  sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 0;
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = 1;
  sd.OutputWindow = gWindowPtr;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sd.Flags = 0;

  HRESULT hr = D3D11CreateDeviceAndSwapChain
  (
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    0,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &sd,
    &gSwapChain,
    &gDevice,
    nullptr,
    &gDeviceContext
  );

  if (FAILED(hr)) {
    return false;
  }


  D3D11_VIEWPORT viewportDesc;

  viewportDesc.Width = static_cast<float>(width);
  viewportDesc.Height = static_cast<float>(height);
  viewportDesc.MinDepth = 0.0f;
  viewportDesc.MaxDepth = 1.0f;
  viewportDesc.TopLeftX = 0;
  viewportDesc.TopLeftY = 0;

  gDeviceContext->RSSetViewports(1, &viewportDesc);

  return true;
}

bool
initBackbufferAndDSV(int width, int height)
{
  HRESULT hr = gSwapChain->GetBuffer(0,
                                  __uuidof(ID3D11Texture2D),
                                  reinterpret_cast<LPVOID*>(&gTextureResource));
  if (FAILED(hr)) {
    DX11SAFE_RELEASE(gTextureResource);
    return false;
  }

  hr = gDevice->CreateRenderTargetView(gTextureResource,
                                      nullptr,
                                      &gBackBufferRTV);
  DX11SAFE_RELEASE(gTextureResource);

  if (FAILED(hr)) {
    DX11SAFE_RELEASE(gBackBufferRTV);
    return false;
  }


  D3D11_TEXTURE2D_DESC descText;
  memset(&descText, 0, sizeof(descText));
  descText.Width = static_cast<UINT>(width);
  descText.Height = static_cast<UINT>(height);
  descText.MipLevels = 0u;
  descText.ArraySize = 1;
  descText.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  descText.SampleDesc.Count = 1;
  descText.SampleDesc.Quality = 0;
  descText.Usage = D3D11_USAGE_DEFAULT;
  descText.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
  descText.CPUAccessFlags = 0;
  descText.MiscFlags = 0;
  hr = gDevice->CreateTexture2D(&descText, nullptr, &gTextureResource);
  if (FAILED(hr)) {
    DX11SAFE_RELEASE(gTextureResource);
    return false;
  }

  D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
  memset(&descDSV, 0, sizeof(descDSV));
  descDSV.Format = descText.Format;
  descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  descDSV.Texture2D.MipSlice = 0;
  hr = gDevice->CreateDepthStencilView(gTextureResource,
                                      &descDSV,
                                      &gDepthStencilView);
  DX11SAFE_RELEASE(gTextureResource);

  if (FAILED(hr)) {
    DX11SAFE_RELEASE(gDepthStencilView);
    return false;
  }

  gDeviceContext->OMSetRenderTargets(1,
                                     &gBackBufferRTV,
                                     gDepthStencilView);

  return true;
}

bool
initDX11()
{
  /****************************************************************************/
  /*                                                                          */
  /*                               Init Window                                */
  /*                                                                          */
  /****************************************************************************/
  if (!initWindowDX11(gWindowWidth, gWindowHeight)) return false;


  RECT rc = {};

  GetClientRect(gWindowPtr, &rc);
  int width = rc.right - rc.left;
  int height = rc.bottom - rc.top;

  /****************************************************************************/
  /*                                                                          */
  /*                               Init basics                                */
  /*                                                                          */
  /****************************************************************************/  
  if (!initBasics(width, height)) return false;



  /****************************************************************************/
  /*                                                                          */
  /*                     Init back buffer & depth stencil                     */
  /*                                                                          */
  /****************************************************************************/
  if (!initBackbufferAndDSV(width, height)) return false;

  return true;
}

void
renderDX11()
{
  float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  gDeviceContext->ClearRenderTargetView(gBackBufferRTV, color);
  gDeviceContext->ClearDepthStencilView(gDepthStencilView,
                                        D3D11_CLEAR_DEPTH,
                                        1.0f,
                                        0);
}

void
releaseDX11()
{
  DX11SAFE_RELEASE(gDevice);
  DX11SAFE_RELEASE(gDeviceContext);
  DX11SAFE_RELEASE(gSwapChain);
  DX11SAFE_RELEASE(gTextureResource);
  DX11SAFE_RELEASE(gBackBufferRTV);
}

void
resizeDX11(int width, int height)
{
  gWindowWidth = width;
  gWindowHeight = height;
  //releaseDX11();

  D3D11_VIEWPORT viewportDesc;

  viewportDesc.Width = static_cast<float>(width);
  viewportDesc.Height = static_cast<float>(height);
  viewportDesc.MinDepth = 0.0f;
  viewportDesc.MaxDepth = 1.0f;
  viewportDesc.TopLeftX = 0;
  viewportDesc.TopLeftY = 0;

  gDeviceContext->RSSetViewports(1, &viewportDesc);
  //initBasics(width, height);
  //initBackbufferAndDSV(width, height);
}


void
initImGui()
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer back ends
  ImGui_ImplDX11_Init(gDevice, gDeviceContext);
  ImGui_ImplWin32_Init(gWindowPtr);
}


void
getReducedAudioData(const Sound* data,
                    U32 newCount,
                    float** outData,
                    U32* outCount)
{
  float interval = static_cast<float>(data->count) / newCount;

  *outCount = newCount;
  *outData = new float[*outCount];
  for (U32 i = 0; i < data->count; ++i) {
    auto ni = static_cast<U32>(interval * i);
    if (ni >= data->count || i >= *outCount) break;

    (*outData)[i] = data->data[i];
  }
}

template<typename T>
T
getMin(T a, T b)
{
  return a < b ? a : b;
}

void
drawChannel(SoundMixerChannel* chann, int& pushID)
{
  float startPosY = ImGui::GetCursorPosY();
  float startPosX= ImGui::GetCursorPosX();
  ImGui::Text(chann->getName().c_str());

  ImGui::PushID(pushID++);
  ImGui::PushItemWidth(gChannelItemsWidth);
  float volume = chann->getVolume();
  if (ImGui::DragFloat("##Volume", &volume, 0.01f, 0.0f, 2.0f)) {
    chann->setVolume(volume);
  }
  ImGui::PopItemWidth();
  ImGui::Text("Volume");

  ImGui::PushItemWidth(gChannelItemsWidth);
  float pan = chann->getPan();
  if (ImGui::DragFloat("##Pan", &pan, 0.01f, -1.0f, 1.0f)) {
    chann->setPan(pan);
  }
  ImGui::PopItemWidth();
  ImGui::Text("Pan");

  bool playOnStart = chann->getPlayOnStart();
  if (ImGui::Checkbox("##Play on Start", &playOnStart)) {
    chann->setPlayOnStart(playOnStart);
  }
  ImGui::Text("Play on Start");

  if (chann->getIsPaused() || !chann->getIsPlaying()) {
    if (ImGui::Button("Play")) {
      chann->play();
    }
  }
  else {
    if (ImGui::Button("Pause")) {
      chann->pause();
    }
  }


  ImVec2 winPos = ImGui::GetWindowPos();
  ImVec2 winSize = ImGui::GetWindowSize();
  float lastPosY = ImGui::GetCursorPosY();
  ImDrawList* DrawList = ImGui::GetWindowDrawList();

  ImVec2 TLC = ImVec2(winPos.x + startPosX + gChannelItemsWidth + gChannelTLOffset.x,
                      winPos.y + startPosY + gChannelTLOffset.y);
  ImVec2 BRC = ImVec2(winPos.x + winSize.x - gChannelBROffset.x,
                      winPos.y + lastPosY - gChannelBROffset.y);
  DrawList->AddRectFilled(TLC, BRC, 0xA0FF8080);

  const auto& tracks = chann->getAudioTracks();
  Vector<ImVec2> trackCorners;
  Vector<ImVec2> trackPoints;
  trackCorners.resize(5);
  for (const auto& t : tracks) {
    auto trackStart = ImVec2(getMin(TLC.x
                                  + t.getStartPositionSec()
                                  * DEFAULT_PIXELS_PER_SECOND
                                  * gZoom[gChannIndex],
                                    BRC.x),
                             TLC.y + 10.0f);
    auto trackEnd = ImVec2(getMin(TLC.x
                                + (t.getStartPositionSec() + t.getMaxPositionSec())
                                * DEFAULT_PIXELS_PER_SECOND
                                * gZoom[gChannIndex],
                                  BRC.x),
                           BRC.y - 10.0f);
    DrawList->AddRectFilled(trackStart, trackEnd, 0xC00000B6);

    trackCorners[0] = trackStart;
    trackCorners[1] = ImVec2(trackStart.x, trackEnd.y);
    trackCorners[2] = trackEnd;
    trackCorners[3] = ImVec2(trackEnd.x, trackStart.y);

    DrawList->AddPolyline(trackCorners.data(), 4, 0xFF00A0FF, true, 2.0f);

    float trackWidth = trackEnd.x - trackStart.x;
    float dx = trackWidth / (1000 - 1);
    float trackHeight = (trackEnd.y - trackStart.y) * 0.5f;
    float trackMidY = trackStart.y + trackHeight;
    trackHeight -= 10.0f;
    float* values = nullptr;
    U32 count = 0;
    getReducedAudioData(t.getSound(), 1000, &values, &count);
    trackPoints.resize(1000);
    for (U32 i = 0; i < 1000; ++i) {
      trackPoints[i] = ImVec2(trackStart.x + dx * i, trackMidY + values[i] * trackHeight);
    }
    DrawList->AddPolyline(trackPoints.data(), 1000, 0xFF000000, false, 1.0f);

    delete[] values;

  }

  float pos = chann->getTimePositionSec();
  DrawList->AddLine(
  ImVec2(getMin(TLC.x
              + pos
              * DEFAULT_PIXELS_PER_SECOND
              * gZoom[gChannIndex],
                BRC.x),
         TLC.y - 10.0f),
  ImVec2(getMin(TLC.x
              + pos
              * DEFAULT_PIXELS_PER_SECOND
              * gZoom[gChannIndex],
                BRC.x),
         BRC.y + 10.0f),
  0xFF00FFFF, 3.0f);

  const auto& events = chann->getEvents();
  for (const auto& e : events) {
    float eventStartPos = e->getFirstTime() / DEF_FREQ;
    float startX = getMin(TLC.x
                        + eventStartPos
                        * DEFAULT_PIXELS_PER_SECOND
                        * gZoom[gChannIndex],
                          BRC.x);
    DrawList->AddLine(
      ImVec2(startX, TLC.y - 10.0f),
      ImVec2(startX, BRC.y + 10.0f),
      0xFF00FF00, 2.0f);
    float eventLastPos = e->getLastTime() / DEF_FREQ;
    if (eventStartPos != eventLastPos) {
      float lastX = getMin(TLC.x
                         + eventLastPos
                         * DEFAULT_PIXELS_PER_SECOND
                         * gZoom[gChannIndex],
                           BRC.x);
      DrawList->AddLine(
        ImVec2(lastX, TLC.y - 10.0f),
        ImVec2(lastX, BRC.y + 10.0f),
        0xFF00FF00, 2.0f);

      DrawList->AddRectFilled(
        ImVec2(startX, TLC.y),
        ImVec2(lastX, BRC.y),
        0xA00000FF);
    }
  }

  ImGui::PopID();
}
void
drawSoundMixerUI(int& pushID)
{
  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoTitleBar;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;
  window_flags |= ImGuiWindowFlags_NoCollapse;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

  static bool unsavedDoc = false;
  if (unsavedDoc) window_flags |= ImGuiWindowFlags_UnsavedDocument;

  if (ImGui::Begin("Echo Adaptive Tool. V 0.1b", nullptr, window_flags)) {
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetWindowSize(ImVec2(static_cast<float>(gWindowWidth),
                                static_cast<float>(gWindowHeight)));
    if (gMixer.getIsPaused() || !gMixer.getIsPlaying()) {
      if (ImGui::Button("Play")) {
        gMixer.play();
      }
    }
    else {
      if (ImGui::Button("Pause")) {
        gMixer.pause();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
      gMixer.stop();
    }

    U32 channelCount = gMixer.getChannelCount();

    ImGui::Separator();
    for (U32 i = 0; i < channelCount; ++i) {
      gChannIndex = i;
      ImGui::PushID(pushID++);
      auto chann = gMixer.getChannel(i);

      ImGui::Indent();

      drawChannel(chann, pushID);

      ImGui::PopID();
      ImGui::Unindent();
      ImGui::Separator();
    }
  }
  ImGui::End();



  //ImGui::ShowDemoWindow();
}

SoundMixerSequencer gSequence;
void
drawSequencerTest()
{
  if (ImGui::Begin("SequencerTestWindow"))
  {
    // let's create the sequencer
    static int selectedEntry = -1;
    static int firstFrame = 0;
    static bool expanded = true;
    static int currentFrame = 100;

    if (gMixer.getIsPaused() || !gMixer.getIsPlaying()) {
      if (ImGui::Button("Play", { 40.0f, 20.0f })) {
        gMixer.play();
      }
    }
    else {
      if (ImGui::Button("Pause", { 40.0f, 20.0f })) {
        gMixer.pause();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop", { 40.0f, 20.0f })) {
      gMixer.stop();
    }
    ImGui::PushItemWidth(130);
    ImGui::InputInt("Frame Min", &gSequence.m_frameMin);
    ImGui::SameLine();
    ImGui::InputInt("Frame ", &currentFrame);
    ImGui::SameLine();
    ImGui::InputInt("Frame Max", &gSequence.m_frameMax);
    ImGui::PopItemWidth();
    Sequencer(&gSequence, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);
    // add a UI to edit that particular item
    if (selectedEntry != -1)
    {
      //const auto& item = gSequence.myItems[selectedEntry];
      //ImGui::Text("I am a %s, please edit me", eaSdkEngine::SequencerItemTypeNames[item.type]);
      // switch (type) ....
    }
  }
  ImGui::End();
}

void
uiRender()
{
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  int pushID = 0;
  drawSoundMixerUI(pushID);

  drawSequencerTest();

  ImGui::Render();

  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}



FMOD_RESULT F_CALLBACK
WriteSoundData(FMOD_SOUND* /*sound*/, void* data, unsigned int length)
{
  memset(data, 0, length);
  float* pcmData = (float*)data;
  int pcmDataCount = length / sizeof(float);

  gMixer.writeSoundData(pcmData, pcmDataCount);
  return FMOD_OK;
}

bool
initFMOD()
{
  FMOD_RESULT r;
  FMOD::System* m_fmodSystem = nullptr;
  int m_maxChannels = 64;

  // Create fmod system
  r = FMOD::System_Create(&m_fmodSystem);
  FMOD_ERROR_CHECK(r);

  // Initialize system
  r = m_fmodSystem->init(m_maxChannels, FMOD_INIT_NORMAL, nullptr);
  FMOD_ERROR_CHECK(r);

  // Create and initialize a sound info structure
  FMOD_CREATESOUNDEXINFO info;
  memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
  info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
  // Specify sampling rate, format, and number of channels to use
  // In this case, 44100 Hz, signed 16-bit PCM, Stereo
  info.defaultfrequency = DEF_FREQ;
  info.format = FMOD_SOUND_FORMAT_PCMFLOAT;
  info.numchannels = 2;
  // Size of the entire sound in bytes. Since the sound will be
  // looping, it does not need to be too long. In this example
  // we will be using the equivalent of a 5 seconds sound.
  // i.e. sampleRate * channels * bytesPerSample * durationInSeconds
  info.length = DEF_FREQ * 2 * sizeof(float) * 5;
  // Number of samples we will be submitting at a time
  // A smaller value results in less latency between operations
  // but if it is too small we get problems in the sound
  // In this case we will aim for a latency of 100ms
  // i.e. sampleRate * durationInSeconds = 44100 * 0.1 = 4410
  info.decodebuffersize = static_cast<U32>(DEF_FREQ * 0.1f);
  // Specify the callback function that will provide the audio data
  info.pcmreadcallback = WriteSoundData;


  // Create a looping stream with FMOD_OPENUSER and the info we filled
  FMOD::Sound* m_Sound;
  FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_OPENUSER;
  m_fmodSystem->createStream(0, mode, &info, &m_Sound);
  m_fmodSystem->playSound(m_Sound, 0, false, 0);

  return true;
}

void
pitchSound(Sound* pitchSound, float pitch)
{
  for (U32 i = 0; i < pitchSound->count; ++i) {
    if (static_cast<U32>(pitch * i) >= pitchSound->count) break;

    pitchSound->data[i] = pitchSound->data[static_cast<U32>(pitch * i)];
  }
  pitchSound->count = static_cast<U32>(pitchSound->count / pitch);
}

int
main()
{
  if (!initDX11()) {
    return -1;
  }
  if (!initFMOD()) {
    return -1;
  }

  initImGui();



  Sound sound("LITTLE MISS PERFECT.wav");
  Sound sound2("ActivateLeverSound.wav");
  Sound sound3("ActivateLeverSound.wav");
  pitchSound(&sound3, 1.2f);
  Sound NoteC("C.wav");
  Sound NoteCS("C.wav");
  pitchSound(&NoteCS, pow(2.0f, 1.0f / 12.0f));
  Sound NoteD("C.wav");
  pitchSound(&NoteD, pow(2.0f, 2.0f / 12.0f));
  Sound NoteDS("C.wav");
  pitchSound(&NoteDS, pow(2.0f, 3.0f / 12.0f));
  Sound NoteE("C.wav");
  pitchSound(&NoteE, pow(2.0f, 4.0f / 12.0f));
  Sound NoteF("C.wav");
  pitchSound(&NoteF, pow(2.0f, 5.0f / 12.0f));
  Sound NoteFS("C.wav");
  pitchSound(&NoteFS, pow(2.0f, 6.0f / 12.0f));
  Sound NoteG("C.wav");
  pitchSound(&NoteG, pow(2.0f, 7.0f / 12.0f));
  Sound NoteGS("C.wav");
  pitchSound(&NoteGS, pow(2.0f, 8.0f / 12.0f));
  Sound NoteA("C.wav");
  pitchSound(&NoteA, pow(2.0f, 9.0f / 12.0f));
  Sound NoteAS("C.wav");
  pitchSound(&NoteAS, pow(2.0f, 10.0f / 12.0f));
  Sound NoteB("C.wav");
  pitchSound(&NoteB, pow(2.0f, 11.0f / 12.0f));



  float bit = 0.25f;

  auto channMix1 = gMixer.addChannel("Percussion");
  channMix1->addTrack(&sound3, bit * 0);
  channMix1->addTrack(&sound2, bit * 1);
  channMix1->addTrack(&sound2, bit * 2);
  channMix1->addTrack(&sound3, bit * 3);
  channMix1->addTrack(&sound2, bit * 4);
  channMix1->addTrack(&sound2, bit * 5);
  channMix1->addTrack(&sound3, bit * 6);
  channMix1->addTrack(&sound2, bit * 7);
  auto loopEvent = new SoundEventLoop();
  loopEvent->setVariable<float>("StartLoopTime", 0.0f);
  channMix1->addEvent(loopEvent, bit * 8);


  auto loopEvent2 = new SoundEventLoop();
  loopEvent2->setVariable<float>("StartLoopTime", bit * 32);

  auto channMix2 = gMixer.addChannel("Chords 1");
  channMix2->addTrack(&NoteF, bit * 32);
  channMix2->addTrack(&NoteD, bit * 48);
  channMix2->addTrack(&NoteG, bit * 64);
  channMix2->addTrack(&NoteAS, bit * 80);
  channMix2->addTrack(&NoteC, bit * 88);

  channMix2->addEvent(loopEvent2, bit * 96);


  auto channMix3 = gMixer.addChannel("Chords 2");
  channMix3->addTrack(&NoteA, bit * 32);
  channMix3->addTrack(&NoteF, bit * 48);
  channMix3->addTrack(&NoteAS, bit * 64);
  channMix3->addTrack(&NoteD, bit * 80);
  channMix3->addTrack(&NoteE, bit * 88);

  channMix3->addEvent(loopEvent2, bit * 96);


  auto channMix4 = gMixer.addChannel("Chords 3");
  channMix4->addTrack(&NoteC, bit * 32);
  channMix4->addTrack(&NoteA, bit * 48);
  channMix4->addTrack(&NoteD, bit * 64);
  channMix4->addTrack(&NoteF, bit * 80);
  channMix4->addTrack(&NoteG,  bit * 88);

  channMix4->addEvent(loopEvent2, bit * 96);





  gSequence.m_frameMin = 0;
  gSequence.m_frameMax = 1500000;
  gSequence.myItems.push_back(SoundMixerSequencer::MySequenceItem{ 0, 10, 30, false });
  gSequence.myItems.push_back(SoundMixerSequencer::MySequenceItem{ 1, 20, 30, true });
  gSequence.myItems.push_back(SoundMixerSequencer::MySequenceItem{ 3, 12, 60, false });
  gSequence.myItems.push_back(SoundMixerSequencer::MySequenceItem{ 2, 61, 90, false });
  gSequence.myItems.push_back(SoundMixerSequencer::MySequenceItem{ 4, 90, 99, false });

  gSequence.setMixerPtr(&gMixer);




  MSG msg = { nullptr };
  while (WM_QUIT != msg.message) {
    renderDX11();

    uiRender();

    gSwapChain->Present(0u, 0u);

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  delete loopEvent;
  delete loopEvent2;


  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  releaseDX11();
}