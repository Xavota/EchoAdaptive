#pragma once
#include "eaPrerrequisitesUtilities.h"

namespace eaSdkEngine {
struct Sound
{
  Sound() = default;
  Sound(const char* path);
  ~Sound();
  U32 samplingRate;
  U16 numChannels;
  U16 bitsPerSample;
  float* data;
  U32 count;
};
}