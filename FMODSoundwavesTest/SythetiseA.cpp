#include "SythetiseA.h"
#include <iostream>

void
SynthetiseA::getData(float* pData, U32 pos)
{
  if (!pData) return;

  SynthetiseSound::getData(pData, pos);

  float val = sin(x * 2756);
  pData[pos] += val;
  pData[pos + 1] += val;
}