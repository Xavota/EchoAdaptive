#include "eeAudioImGuiImpl.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <string>

#include "SoundMixer.h"
#include "SoundMixerChannel.h"
#include "FMODSound.h"

void
getReducedAudioData(const FMODSound const* data, int newCount, float** outData, int* outCount)
{
  float interval = static_cast<float>(data->count) / newCount;

  *outCount = newCount;
  *outData = new float[*outCount];
  for (int i = 0; i < data->count; ++i) {
    auto ni = static_cast<int>(interval * i);
    if (ni >= data->count || i >= *outCount) break;

    (*outData)[i] = data->data[i];
  }
}
void
drawChannel(SoundMixerChannel* chann, ImVec2 tlPos, int& pushID)
{
  //ImGui::PushItemWidth(100);
  //ImGui::DragFloat("float##1b", &f);
  //if (show_indented_items)
  //{
  //  ImGui::Indent();
  //  ImGui::DragFloat("float (indented)##1b", &f);
  //  ImGui::Unindent();
  //}
  //ImGui::PopItemWidth();


  ImGui::PushItemWidth(100);
  float volume = chann->getVolume();
  if (ImGui::DragFloat("Volume", &volume, 0.01f, 0.0f, 2.0f)) {
    chann->setVolume(volume);
  }
  ImGui::PopItemWidth();

  ImGui::PushItemWidth(100);
  float pan = chann->getPan();
  if (ImGui::DragFloat("Pan", &pan, 0.01f, -1.0f, 1.0f)) {
    chann->setPan(pan);
  }
  ImGui::PopItemWidth();

  const auto& tracks = chann->getAudioTracks();
  if (!tracks.empty()) {
    ImGui::PushItemWidth(100);
    float* values = nullptr;
    int count = 0;
    getReducedAudioData(tracks[0].getSound(), 5000, &values, &count);
    ImGui::PlotLines("## Sound Wave", values, count);
    delete[] values;
    ImGui::PopItemWidth();
  }


  //ImGui::pos


  ImVec2 winPos = ImGui::GetWindowPos();
  ImVec2 winSize = ImGui::GetWindowSize();
  ImDrawList* DrawList = ImGui::GetWindowDrawList();

  //const char* testText = "Test text";
  //DrawList->AddText(ImVec2(winPos.x + 100.0f, winPos.y + 100.0f), 0xFFFFFFFF, "Test text");

  DrawList->AddRectFilled(ImVec2(winPos.x + tlPos.x + 200.0f, winPos.y + tlPos.y + 5.0f), 
                          ImVec2(winPos.x + winSize.x - 10.0f, winPos.y + tlPos.y + 84.0f),
                          0xA0FF8080);
}
void
AudioImGuiImpl::drawUI(SoundMixer* mixerTool, int& pushID)
{
  if (ImGui::Begin("Echo Adaptive Tool. V 0.1b")) {
    uint32_t channelCount = mixerTool->getChannelCount();

    static const ImVec2 channelSize = ImVec2(0.0f, 89.0f);

    ImGui::Separator();
    for (int32_t i = 0; i < channelCount; ++i) {
      ImGui::PushID(pushID++);
      auto chann = mixerTool->getChannel(i);

      ImGui::Indent();

      ImGui::Text(std::to_string(i).c_str());
      //static float offset_x = 0.0f;
      //offset_x += 0.01f;
      //ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)offset_x);

      drawChannel(chann, ImVec2(channelSize.x * i, 29.0f + channelSize.y * i), pushID);

      ImGui::PopID();
      ImGui::Unindent();
      ImGui::Separator();
    }
  }
  ImGui::End();

  
  if (ImGui::Begin("Drag & Drop Test")) {
    // Reordering is actually a rather odd use case for the drag and drop API which is meant to carry data around. 
    // Here we implement a little demo using the drag and drop primitives, but we could perfectly achieve the same results by using a mixture of
    //  IsItemActive() on the source item + IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) on target items.
    // This demo however serves as a pretext to demonstrate some of the flags you can use with BeginDragDropSource() and AcceptDragDropPayload().
    ImGui::BulletText("Drag and drop to re-order");
    ImGui::Indent();
    static const char* names[6] = { "1. Adbul", "2. Alfonso", "3. Aline", "4. Amelie", "5. Anna", "6. Arthur" };
    int move_from = -1, move_to = -1;
    for (int n = 0; n < IM_ARRAYSIZE(names); n++) {
      ImGui::Selectable(names[n]);

      ImGuiDragDropFlags src_flags = 0;
      src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
      src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
      //src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
      if (ImGui::BeginDragDropSource(src_flags)) {
        if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
          ImGui::Text("Moving \"%s\"", names[n]);
        ImGui::SetDragDropPayload("DND_DEMO_NAME", &n, sizeof(int));
        ImGui::EndDragDropSource();
      }

      if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags target_flags = 0;
        target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
        target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", target_flags))
        {
          move_from = *(const int*)payload->Data;
          move_to = n;
        }
        ImGui::EndDragDropTarget();
      }
    }

    if (move_from != -1 && move_to != -1) {
      // Reorder items
      int copy_dst = (move_from < move_to) ? move_from : move_to + 1;
      int copy_src = (move_from < move_to) ? move_from + 1 : move_to;
      int copy_count = (move_from < move_to) ? move_to - move_from : move_from - move_to;
      const char* tmp = names[move_from];
      //printf("[%05d] move %d->%d (copy %d..%d to %d..%d)\n", ImGui::GetFrameCount(), move_from, move_to, copy_src, copy_src + copy_count - 1, copy_dst, copy_dst + copy_count - 1);
      memmove(&names[copy_dst], &names[copy_src], (size_t)copy_count * sizeof(const char*));
      names[move_to] = tmp;
      ImGui::SetDragDropPayload("DND_DEMO_NAME", &move_to, sizeof(int)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
    }
    ImGui::Unindent();
  }
  ImGui::End();



  ImGui::ShowDemoWindow();
}
