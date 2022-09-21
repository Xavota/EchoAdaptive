#pragma once
#include "SoundEffect.h"

class ExpSoundEffect : public SoundEffect
{
public:
  ExpSoundEffect() = default;
  ExpSoundEffect(float _exp) : exp(_exp) {}
  ~ExpSoundEffect() = default;

  void
  applyPreEffect(float* data, U32& count) override;

  float exp = 2.0f;
};