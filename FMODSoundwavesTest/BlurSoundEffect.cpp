#include "BlurSoundEffect.h"

void
BlurSoundEffect::applyPreEffect(float* data, U32& count)
{
  for (int i = 0; i < count; ++i) {
    float totalSum = data[i];
    int sumCount = 1;
    for (int j = 1; j <= rad; ++j) {
      if (i + j < count) {
        totalSum += data[i + j];
        ++sumCount;
      }
      if (i - j >= 0) {
        totalSum += data[i - j];
        ++sumCount;
      }
    }
    data[i] = totalSum / sumCount;
  }
}