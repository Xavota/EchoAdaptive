#pragma once
#include "Prerrequisites.h"

class SoundEffect
{
public:
  SoundEffect() = default;
  ~SoundEffect() = default;

  virtual void
  applyPreEffect(float* /*data*/, U32& /*count*/) {};

  virtual void
  applyPostEffect(float* /*data*/, U32& /*count*/) {};
};

