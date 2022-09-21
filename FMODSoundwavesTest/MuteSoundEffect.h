#pragma once
#include "SoundEffect.h"

class MuteSoundEffect : public SoundEffect
{
public:
  MuteSoundEffect() = default;
  ~MuteSoundEffect() = default;

  void
  applyPostEffect(float* data, U32& count) override;
};