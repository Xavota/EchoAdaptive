#include "eeAudioImGuiImpl.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <string>

#include "SoundMixer.h"
#include "SoundMixerChannel.h"
#include "FMODSound.h"

#include "AudioEvent.h"

#define DEFAULT_PIXELS_PER_SECOND 150

static const ImVec2 gChannelTLOffset = ImVec2(10.0f, 10.0f);
static const ImVec2 gChannelBROffset = ImVec2(10.0f, 10.0f);
static float gChannelItemsWidth = 100.0f;

static const std::vector<float> gZoom = { 3.5f, 0.3f, 0.3f, 0.3f };
static int gChannIndex = 0;

static ImFont gFont;

AudioImGuiImpl::AudioImGuiImpl()
{
  gFont.FontSize = 10.0f;
}
void
getReducedAudioData(const FMODSound* data,
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
void
drawChannel(SoundMixerChannel* chann, int& pushID)
{
  float startPosY = ImGui::GetCursorPosY();
  float startPosX= ImGui::GetCursorPosX();
  ImGui::Text(std::to_string(0).c_str());

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
  std::vector<ImVec2> trackCorners;
  std::vector<ImVec2> trackPoints;
  trackCorners.resize(5);
  for (const auto& t : tracks) {
    auto trackStart = ImVec2(std::min(TLC.x
                                    + t.getStartPositionSec()
                                    * DEFAULT_PIXELS_PER_SECOND
                                    * gZoom[gChannIndex],
                                      BRC.x),
                             TLC.y + 10.0f);
    auto trackEnd = ImVec2(std::min(TLC.x
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
    trackCorners[4] = trackStart;

    DrawList->AddPolyline(trackCorners.data(), 5, 0xFF00A0FF, 0, 2.0f);

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
    DrawList->AddPolyline(trackPoints.data(), 1000, 0xFF000000, 0, 1.0f);

    delete[] values;

  }

  float pos = chann->getTimePositionSec();
  DrawList->AddLine(
  ImVec2(std::min(TLC.x
                + pos
                * DEFAULT_PIXELS_PER_SECOND
                * gZoom[gChannIndex],
                  BRC.x),
         TLC.y - 10.0f),
  ImVec2(std::min(TLC.x
                + pos
                * DEFAULT_PIXELS_PER_SECOND
                * gZoom[gChannIndex],
                  BRC.x),
         BRC.y + 10.0f),
  0xFF00FFFF, 3.0f);

  const auto& events = chann->getEvents();
  for (const auto& e : events) {
    float eventStartPos = e->getFirstTime() / DEF_FREQ;
    float startX = std::min(TLC.x
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
      float lastX = std::min(TLC.x
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
AudioImGuiImpl::drawUI(sf::RenderWindow* window, SoundMixer* mixerTool, int& pushID)
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
    auto winSize = window->getSize();
    ImGui::SetWindowSize(ImVec2(static_cast<float>(winSize.x),
                                static_cast<float>(winSize.y)));
    if (mixerTool->getIsPaused() || !mixerTool->getIsPlaying()) {
      if (ImGui::Button("Play")) {
        mixerTool->play();
      }
    }
    else {
      if (ImGui::Button("Pause")) {
        mixerTool->pause();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
      mixerTool->stop();
    }

    U32 channelCount = mixerTool->getChannelCount();

    ImGui::Separator();
    for (U32 i = 0; i < channelCount; ++i) {
      gChannIndex = i;
      ImGui::PushID(pushID++);
      auto chann = mixerTool->getChannel(i);

      ImGui::Indent();

      //static float offset_x = 0.0f;
      //offset_x += 0.01f;
      //ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)offset_x);

      drawChannel(chann, pushID);

      ImGui::PopID();
      ImGui::Unindent();
      ImGui::Separator();
    }
  }
  ImGui::End();

  
  //if (ImGui::Begin("Drag & Drop Test")) {
  //  // Reordering is actually a rather odd use case for the drag and drop API which is meant to carry data around. 
  //  // Here we implement a little demo using the drag and drop primitives, but we could perfectly achieve the same results by using a mixture of
  //  //  IsItemActive() on the source item + IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) on target items.
  //  // This demo however serves as a pretext to demonstrate some of the flags you can use with BeginDragDropSource() and AcceptDragDropPayload().
  //  ImGui::BulletText("Drag and drop to re-order");
  //  ImGui::Indent();
  //  static const char* names[6] = { "1. Adbul", "2. Alfonso", "3. Aline", "4. Amelie", "5. Anna", "6. Arthur" };
  //  int move_from = -1, move_to = -1;
  //  for (int n = 0; n < IM_ARRAYSIZE(names); n++) {
  //    ImGui::Selectable(names[n]);
  //
  //    ImGuiDragDropFlags src_flags = 0;
  //    src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
  //    src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
  //    //src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
  //    if (ImGui::BeginDragDropSource(src_flags)) {
  //      if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
  //        ImGui::Text("Moving \"%s\"", names[n]);
  //      ImGui::SetDragDropPayload("DND_DEMO_NAME", &n, sizeof(int));
  //      ImGui::EndDragDropSource();
  //    }
  //
  //    if (ImGui::BeginDragDropTarget()) {
  //      ImGuiDragDropFlags target_flags = 0;
  //      target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
  //      target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
  //      if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", target_flags))
  //      {
  //        move_from = *(const int*)payload->Data;
  //        move_to = n;
  //      }
  //      ImGui::EndDragDropTarget();
  //    }
  //  }
  //
  //  if (move_from != -1 && move_to != -1) {
  //    // Reorder items
  //    int copy_dst = (move_from < move_to) ? move_from : move_to + 1;
  //    int copy_src = (move_from < move_to) ? move_from + 1 : move_to;
  //    int copy_count = (move_from < move_to) ? move_to - move_from : move_from - move_to;
  //    const char* tmp = names[move_from];
  //    //printf("[%05d] move %d->%d (copy %d..%d to %d..%d)\n", ImGui::GetFrameCount(), move_from, move_to, copy_src, copy_src + copy_count - 1, copy_dst, copy_dst + copy_count - 1);
  //    memmove(&names[copy_dst], &names[copy_src], (size_t)copy_count * sizeof(const char*));
  //    names[move_to] = tmp;
  //    ImGui::SetDragDropPayload("DND_DEMO_NAME", &move_to, sizeof(int)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
  //  }
  //  ImGui::Unindent();
  //}
  //ImGui::End();



  ImGui::ShowDemoWindow();
}
