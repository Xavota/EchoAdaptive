#include "ExpSoundEffect.h"
#include <iostream>

float
sign(float f)
{
  return f / abs(f);
}
void
ExpSoundEffect::applyPreEffect(float* data, U32& count)
{
  for (U32 i = 0; i < count; ++i) {
    data[i] = pow(abs(data[i]), exp) * sign(data[i]);
  }
}