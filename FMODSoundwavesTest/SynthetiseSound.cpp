#include "SynthetiseSound.h"

void
SynthetiseSound::getData(float* /*pData*/, U32 /*pos*/)
{
  x = static_cast<float>(position) / samplingRate;
  ++position;
}