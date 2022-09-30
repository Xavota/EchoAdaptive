#pragma once
#include "Prerrequisites.h"

struct FMODSound;

class SoundEditor
{
 public:
  SoundEditor() = default;
  ~SoundEditor() = default;

  bool
  cutSoundOnSecond(const FMODSound& soundToCut,
                   float second,
                   FMODSound& outSoundR1,
                   FMODSound& outSoundR2);

  bool
  cutSoundOnFrequency(const FMODSound& soundToCut,
                      U32 frequency,
                      FMODSound& outSoundR1,
                      FMODSound& outSoundR2);

  void
  uniteSounds(const FMODSound& soundToUnite1,
              const FMODSound& soundToUnite2,
              FMODSound& outSoundR);
};