#include "PitchSoundEffect.h"

void
PitchSoundEffect::applyPreEffect(float* data, U32& count)
{
  for (U32 i = 0; i < count; ++i) {
    if (static_cast<U32>(pitch * i) >= count) break;

    data[i] = data[static_cast<U32>(pitch * i)];
  }
  count = static_cast<U32>(count / pitch);
}
