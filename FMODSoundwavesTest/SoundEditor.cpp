#include "SoundEditor.h"

#include <iostream>

#include "FMODSound.h"

bool
SoundEditor::cutSoundOnSecond(const FMODSound& soundToCut,
                              float second,
                              FMODSound& outSoundR1,
                              FMODSound& outSoundR2)
{
  return cutSoundOnFrequency(soundToCut,
                             static_cast<U32>(second
                                            * soundToCut.samplingRate
                                            * soundToCut.numChannels),
                             outSoundR1,
                             outSoundR2);
}

bool
SoundEditor::cutSoundOnFrequency(const FMODSound& soundToCut,
                                 U32 frequency,
                                 FMODSound& outSoundR1,
                                 FMODSound& outSoundR2)
{
  if (soundToCut.data && frequency < soundToCut.count - 1 && frequency >= 0) {
    outSoundR1.samplingRate = soundToCut.samplingRate;
    outSoundR1.numChannels = soundToCut.numChannels;
    outSoundR1.bitsPerSample = soundToCut.bitsPerSample;
    outSoundR1.count = frequency + 1;
    outSoundR1.data = new float[outSoundR1.count];
    memcpy(outSoundR1.data,
           soundToCut.data,
           outSoundR1.count * sizeof(float));

    outSoundR2.samplingRate = soundToCut.samplingRate;
    outSoundR2.numChannels = soundToCut.numChannels;
    outSoundR2.bitsPerSample = soundToCut.bitsPerSample;
    outSoundR2.count = soundToCut.count - frequency - 1;
    outSoundR2.data = new float[outSoundR2.count];
    memcpy(outSoundR2.data,
           soundToCut.data + frequency + 1,
           outSoundR2.count * sizeof(float));

    return true;
  }

  return false;
}

void
SoundEditor::uniteSounds(const FMODSound& soundToUnite1,
                         const FMODSound& soundToUnite2,
                         FMODSound& outSoundR)
{
  outSoundR.samplingRate = soundToUnite1.samplingRate;
  outSoundR.numChannels = soundToUnite1.numChannels;
  outSoundR.bitsPerSample = soundToUnite1.bitsPerSample;
  outSoundR.count = soundToUnite1.count + soundToUnite2.count;
  outSoundR.data = new float[outSoundR.count];
  memcpy(outSoundR.data,
         soundToUnite1.data,
         soundToUnite1.count * sizeof(float));
  memcpy(outSoundR.data + soundToUnite1.count,
         soundToUnite2.data,
         soundToUnite2.count * sizeof(float));
}