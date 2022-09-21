#pragma once
#include "SoundEffect.h"

class PitchSoundEffect : public SoundEffect
{
public:
  PitchSoundEffect() = default;
  PitchSoundEffect(float _pitch) : pitch(_pitch) {}
  ~PitchSoundEffect() = default;

  void
  applyPreEffect(float* data, U32& count) override;

  float pitch = 1.0f;
};

