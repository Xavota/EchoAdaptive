#pragma once
#include "SynthetiseSound.h"

class SynthetiseA : public SynthetiseSound
{
public:
  SynthetiseA() = default;
  ~SynthetiseA() = default;

  void
  getData(float* pData, U32 pos) override;
};

