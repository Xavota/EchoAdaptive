#pragma once
#include "Prerrequisites.h"

class SynthetiseSound
{
 public:
  SynthetiseSound() = default;
  ~SynthetiseSound() = default;

  virtual void
  getData(float* pData, U32 pos);

  U32 samplingRate;
  U16 numChannels;

 protected:
  float x = 0.0f;

 private:
  U32 position = 0;
};

