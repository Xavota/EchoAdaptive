#include "MuteSoundEffect.h"

void
MuteSoundEffect::applyPostEffect(float* data, U32& count)
{
  for (int i = 0; i < count; ++i) {
    data[i] = 0.0f;
  }
}