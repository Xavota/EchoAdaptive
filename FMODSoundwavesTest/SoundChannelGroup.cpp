#include "SoundChannelGroup.h"

void
SoundChannelGroup::WriteSoundData(float* data, int count, float parentPitch)
{
  if (paused) return;

  for (auto& cg : m_childGroups) {
    if (cg) cg->WriteSoundData(data, count, parentPitch * pitch);
  }
  for (auto& cc : m_childChannels) {
    if (cc) cc->WriteSoundData(data, count, parentPitch * pitch);
  }
  for (int i = 0; i < count; i += 2) {
    data[i] *= volume * leftGain;
    data[i + 1] *= volume * rightGain;
  }
}