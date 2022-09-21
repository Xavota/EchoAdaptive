#pragma once
#include "SoundEffect.h"

class LimitSoundEffect : public SoundEffect
{
public:
  LimitSoundEffect() = default;
  LimitSoundEffect(float _limit) : limit(_limit) {}
  ~LimitSoundEffect() = default;

  void
  applyPreEffect(float* data, U32& count) override;

  float limit = 0.4f;
};

