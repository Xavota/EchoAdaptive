#include "LimitSoundEffect.h"
#include <iostream>

void
LimitSoundEffect::applyPreEffect(float* data, U32& count)
{
  for (int i = 0; i < count; ++i) {
    data[i] = abs(data[i]) <= limit
            ? data[i] : limit;
  }
}