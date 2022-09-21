#pragma once
#include "SoundEffect.h"

class BlurSoundEffect : public SoundEffect
{
 public:
  BlurSoundEffect() = default;
  BlurSoundEffect(int _rad) : rad(_rad) {}
  ~BlurSoundEffect() = default;

  void
  applyPreEffect(float* data, U32& count) override;

  int rad = 5;
};