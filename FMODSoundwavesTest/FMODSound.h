#pragma once
#include "Prerrequisites.h"

struct FMODSound
{
  FMODSound() = default;
  FMODSound(const char* path);
  ~FMODSound();
  U32 samplingRate;
  U16 numChannels;
  U16 bitsPerSample;
  float* data;
  U32 count;
};

