#include <fmod.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "FMODSound.h"
#include "SoundChannel.h"
#include "SoundChannelGroup.h"

#include "SythetiseA.h"

#include "SoundEditor.h"
#include "SoundWaveDisplay.h"

#include "SoundMixer.h"

#include "AudioEvent.h"
#include "AudioEventLoop.h"

#include "PitchSoundEffect.h"


#include "eeAudioImGuiImpl.h"

//#include "SoundManager_Fmod.h"

#define FMOD_ERROR_CHECK(FMOD_RESULT)                                          \
if (r != FMOD_OK) {                                                            \
  return -1;                                                                   \
}

SoundMixer mixer;
SoundChannelGroup masterChannelGroup;
FMOD_RESULT F_CALLBACK
WriteSoundData(FMOD_SOUND* sound, void* data, unsigned int length)
{
  // Clear output
  memset(data, 0, length);
  // Get data in the correct format and calculate sample count
  float* pcmData = (float*)data;
  int pcmDataCount = length / sizeof(float);
  // Tell the channel to write to the output
  //masterChannelGroup.WriteSoundData(pcmData, pcmDataCount, 1.0f);

  mixer.writeSoundData(pcmData, pcmDataCount);
  return FMOD_OK;
}



void
DrawChannelSettings(SoundChannel* chan, int& id, int& groupID)
{
  groupID++;

  float position = chan->getPositionSec();
  float maxPosition = chan->getMaxPositionSec();
  bool paused = chan->GetPaused();
  bool loop = chan->GetLoop();
  float volume = chan->GetVolume();
  float pitch = chan->GetPitch();
  float pan = chan->GetPan();

  if (ImGui::TreeNode(("Channel" + std::to_string(groupID)).c_str())) {
    ImGui::PushID(id++);
    if (ImGui::SliderFloat("Position", &position, 0.0f, maxPosition)) {
      chan->seek(position);
    }
    if (ImGui::Checkbox("Paused", &paused)) {
      chan->SetPaused(paused);
    }
    if (ImGui::Checkbox("Loop", &loop)) {
      chan->SetLoop(loop);
    }
    if (ImGui::SliderFloat("Volume", &volume, 0.0f, 2.0f)) {
      chan->SetVolume(volume);
    }
    if (ImGui::SliderFloat("Pitch", &pitch, 0.5f, 2.0f)) {
      chan->SetPitch(pitch);
    }
    if (ImGui::SliderFloat("Pan", &pan, -1.0f, 1.0f)) {
      chan->SetPan(pan);
    }
    ImGui::PopID();
    ImGui::TreePop();
  }
}

void
DrawChanelGroupTree(SoundChannelGroup* chanGroup, int& id, int& groupID)
{
  groupID++;

  if (ImGui::TreeNode(("Channel" + std::to_string(groupID)).c_str())) {
    bool paused = chanGroup->GetPaused();
    float volume = chanGroup->GetVolume();
    float pitch = chanGroup->GetPitch();
    float pan = chanGroup->GetPan();

    ImGui::PushID(id++);
    if (ImGui::Checkbox("Paused", &paused)) {
      chanGroup->SetPaused(paused);
    }
    if (ImGui::SliderFloat("Volume", &volume, 0.0f, 2.0f)) {
      chanGroup->SetVolume(volume);
    }
    if (ImGui::SliderFloat("Pitch", &pitch, 0.5f, 2.0f)) {
      chanGroup->SetPitch(pitch);
    }
    if (ImGui::SliderFloat("Pan", &pan, -1.0f, 1.0f)) {
      chanGroup->SetPan(pan);
    }
    ImGui::PopID();

    auto& chanGroups = chanGroup->getChannelGroupChildren();
    for (auto& cg : chanGroups) {
      DrawChanelGroupTree(cg, id, groupID);
    }
    auto& chanChildren = chanGroup->getChannelChildren();
    for (auto& cc : chanChildren) {
      DrawChannelSettings(cc, id, groupID);
    }
    ImGui::TreePop();
  }
}

struct ImTextQueue
{
  float posX;
  float posY;
  std::string text;
};

void
DrawImgui(sf::RenderWindow* window, sf::Time deltaTime)
{
  ImGui::SFML::Update(*window, deltaTime);
  
  int pushId = 0;

  static AudioImGuiImpl audioImGui;
  audioImGui.drawUI(&mixer, pushId);

  //if (ImGui::Begin("Hello, world!")) {
  //  int id = 0;
  //  int groupID = 0;
  //  DrawChanelGroupTree(&masterChannelGroup, id, groupID);
  //}
  //ImGui::End();

  std::vector<ImTextQueue> TextQueue;
  TextQueue.push_back({110.0f, 150.0f, "Test Text"});

  //ImGui::SetNextWindowPos(ImVec2(100, 100));
  //ImGui::SetNextWindowSize(ImVec2(200, 200));
  if (ImGui::Begin("GlobalScreen", nullptr))
  {
    ImDrawList* DrawList = ImGui::GetWindowDrawList();

    ImVec2 winPos = ImGui::GetWindowPos();

    for (const auto& Text : TextQueue)
    {
      DrawList->AddText(ImVec2(winPos.x + (float)Text.posX, winPos.y + (float)Text.posY), 0xFFFFFFFF, Text.text.c_str());
    }
    TextQueue.clear();
  }
  ImGui::End();
}



int
main()
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



  FMODSound sound("LITTLE MISS PERFECT.wav");
  auto pitchEff = new PitchSoundEffect(1.2f);
  FMODSound sound2("ActivateLeverSound.wav");
  FMODSound sound3("ActivateLeverSound.wav");
  pitchEff->applyPreEffect(sound3.data, sound3.count);
  FMODSound NoteC("C.wav");
  FMODSound NoteCS("C.wav");
  pitchEff->pitch = pow(2.0f, 1.0f / 12.0f);
  pitchEff->applyPreEffect(NoteCS.data, NoteCS.count);
  FMODSound NoteD("C.wav");
  pitchEff->pitch = pow(2.0f, 2.0f / 12.0f);
  pitchEff->applyPreEffect(NoteD.data, NoteD.count);
  FMODSound NoteDS("C.wav");
  pitchEff->pitch = pow(2.0f, 3.0f / 12.0f);
  pitchEff->applyPreEffect(NoteDS.data, NoteDS.count);
  FMODSound NoteE("C.wav");
  pitchEff->pitch = pow(2.0f, 4.0f / 12.0f);
  pitchEff->applyPreEffect(NoteE.data, NoteE.count);
  FMODSound NoteF("C.wav");
  pitchEff->pitch = pow(2.0f, 5.0f / 12.0f);
  pitchEff->applyPreEffect(NoteF.data, NoteF.count);
  FMODSound NoteFS("C.wav");
  pitchEff->pitch = pow(2.0f, 6.0f / 12.0f);
  pitchEff->applyPreEffect(NoteFS.data, NoteFS.count);
  FMODSound NoteG("C.wav");
  pitchEff->pitch = pow(2.0f, 7.0f / 12.0f);
  pitchEff->applyPreEffect(NoteG.data, NoteG.count);
  FMODSound NoteGS("C.wav");
  pitchEff->pitch = pow(2.0f, 8.0f / 12.0f);
  pitchEff->applyPreEffect(NoteGS.data, NoteGS.count);
  FMODSound NoteA("C.wav");
  pitchEff->pitch = pow(2.0f, 9.0f / 12.0f);
  pitchEff->applyPreEffect(NoteA.data, NoteA.count);
  FMODSound NoteAS("C.wav");
  pitchEff->pitch = pow(2.0f, 10.0f / 12.0f);
  pitchEff->applyPreEffect(NoteAS.data, NoteAS.count);
  FMODSound NoteB("C.wav");
  pitchEff->pitch = pow(2.0f, 11.0f / 12.0f);
  pitchEff->applyPreEffect(NoteB.data, NoteB.count);
  delete pitchEff;



  float bit = 0.25f;
  //uint32_t channMix1 = mixer.addChannel();
  //mixer.setChannelPaused(channMix1, false);
  //mixer.addChannelTrack(channMix1, &sound);
  auto channMix2 = mixer.addChannel();
  channMix2->addTrack(&sound3, bit * 0);
  channMix2->addTrack(&sound2, bit * 1);
  channMix2->addTrack(&sound2, bit * 2);
  channMix2->addTrack(&sound3, bit * 3);
  channMix2->addTrack(&sound2, bit * 4);
  channMix2->addTrack(&sound2, bit * 5);
  channMix2->addTrack(&sound3, bit * 6);
  channMix2->addTrack(&sound2, bit * 7);
  AudioEventLoop* loopEvent = new AudioEventLoop();
  loopEvent->setVariable<float>("StartLoopTime", 0.0f);
  channMix2->addEvent(loopEvent, bit * 8);


  auto channMix3 = mixer.addChannel();
  channMix3->addTrack(&NoteF, bit * 32);
  channMix3->addTrack(&NoteA, bit * 32);
  channMix3->addTrack(&NoteC, bit * 32);

  channMix3->addTrack(&NoteD, bit * 48);
  channMix3->addTrack(&NoteF, bit * 48);
  channMix3->addTrack(&NoteA, bit * 48);

  channMix3->addTrack(&NoteG, bit * 64);
  channMix3->addTrack(&NoteAS, bit * 64);
  channMix3->addTrack(&NoteD, bit * 64);

  channMix3->addTrack(&NoteAS, bit * 80);
  channMix3->addTrack(&NoteD, bit * 80);
  channMix3->addTrack(&NoteF, bit * 80);

  channMix3->addTrack(&NoteC, bit * 88);
  channMix3->addTrack(&NoteE, bit * 88);
  channMix3->addTrack(&NoteG,  bit * 88);
  AudioEventLoop* loopEvent2 = new AudioEventLoop();
  loopEvent2->setVariable<float>("StartLoopTime", bit * 32);
  channMix3->addEvent(loopEvent2, bit * 96);

  //auto channMix4 = mixer.addChannel();







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
  info.decodebuffersize = DEF_FREQ * 0.1f;
  // Specify the callback function that will provide the audio data
  info.pcmreadcallback = WriteSoundData;


  // Create a looping stream with FMOD_OPENUSER and the info we filled
  FMOD::Sound* m_fmodSound;
  FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_OPENUSER;
  m_fmodSystem->createStream(0, mode, &info, &m_fmodSound);
  m_fmodSystem->playSound(m_fmodSound, 0, false, 0);









  float wheelDelta = 0.0f;
  bool ctrlPressed = false;
  bool shiftPressed = false;

  sf::Vector2u screenSize {1280, 720};
  sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "SFML works!");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);

  SoundWaveDisplay soundDisp(screenSize);
  soundDisp.addSoundToDisplay(&sound);
  //soundDisp.addSoundToDisplay(&sound2);
  //soundDisp.addSoundToDisplay(&sound1_1);
  //soundDisp.addSoundToDisplay(&sound1_2);
  //soundDisp.addSoundToDisplay(&sound1a2);

  sf::Clock deltaClock;
  while (window.isOpen()) {
    wheelDelta = 0.0f;
    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(window, event);

      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::MouseWheelMoved) {
        wheelDelta = event.mouseWheel.delta;
      }
      else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::LControl) {
          ctrlPressed = true;
        }
        else if (event.key.code == sf::Keyboard::LShift) {
          shiftPressed = true;
        }
      }
      else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::LControl) {
          ctrlPressed = false;
        }
        else if (event.key.code == sf::Keyboard::LShift) {
          shiftPressed = false;
        }
      }
    }

    sf::Time deltaTime = deltaClock.restart();
    DrawImgui(&window, deltaTime);

    mixer.update(deltaTime.asSeconds());

    if (ctrlPressed) {
      soundDisp.zoom(wheelDelta);
    }
    else if (shiftPressed) {
      soundDisp.expand(wheelDelta);
    }
    else {
      soundDisp.scroll(wheelDelta);
    }

    if (m_fmodSystem) m_fmodSystem->update();

    window.clear();

    soundDisp.render(&window);

    ImGui::SFML::Render(window);
    window.display();
  }

  //delete[] normalizedArray;
  //delete eff;
  //delete eff2;
  //delete eff3;

  //delete synthSound1;

  delete loopEvent;
  delete loopEvent2;

  ImGui::SFML::Shutdown();
  return 0;
}
