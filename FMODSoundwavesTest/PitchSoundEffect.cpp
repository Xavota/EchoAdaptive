#include "PitchSoundEffect.h"

void
PitchSoundEffect::applyPreEffect(float* data, U32& count)
{
  for (int i = 0; i < count; ++i) {
    if (static_cast<int>(pitch * i) >= count) break;

    data[i] = data[static_cast<int>(pitch * i)];
  }
  count /= pitch;
}
