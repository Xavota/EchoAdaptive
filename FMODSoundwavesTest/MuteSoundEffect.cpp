#include "MuteSoundEffect.h"

void
MuteSoundEffect::applyPostEffect(float* data, U32& count)
{
  for (U32 i = 0; i < count; ++i) {
    data[i] = 0.0f;
  }
}