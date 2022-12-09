#pragma once
#include "eaPrerrequisitesUtilities.h"

#include "eaSound.h"

namespace eaSdkEngine {
struct Sound;

class SoundTrack
{
public:
  SoundTrack() :
  sound(nullptr),
  position(0.0f),
  volume(1.0f)
  {
    setPan(0.0f);
  }
  ~SoundTrack()
  {
    sound = nullptr;
  }
  void
  setSound(Sound* mySound)
  {
    sound = mySound;
  }
  inline const Sound*
  getSound() const
  {
    return sound;
  }
  inline Sound*
  getSoundPtr() // TODO: Find a way to do this better. Can't be two functions that do the same. Use SPtr or something to keep track of the sounds instances.
  {
    return sound;
  }
  void
  setStartPosition(float startPCM)
  {
    startPosition = startPCM;
  }
  float
  getStartPosition() const
  {
    return startPosition;
  }
  float
  getStartPositionSec() const
  {
    return startPosition / DEF_FREQ;
  }
  void
  writeSoundData(float* data, int i);
  void
  getSoundData(float& ldata, float& rdata);

  float
  getVolume() const { return volume; }
  void
  setVolume(float value)
  {
    volume = value;
  }

  float
  getPan() const { return pan; }
  void
  setPan(float value)
  {
    if (value < -1.0f) pan = -1.0f;
    else if (value > 1.0f) pan = 1.0f;
    else pan = value;
    updatePan();
  }

  float
  getPositionSec() const
  {
    if (sound) {
      return position / sound->samplingRate;
    }
    else {
      return position / DEF_FREQ;
    }
  }
  float
  getPositionFreq() const
  {
    return position;
  }
  float
  getMaxPositionSec() const
  {
    if (sound) {
      return static_cast<float>(sound->count) / (sound->samplingRate * sound->numChannels);
    }
    else {
      return 0.0f;
    }
  }
  float
  getMaxPositionFreq() const
  {
    if (sound) {
      return static_cast<float>(sound->count / sound->numChannels);
    }
    else {
      return 0.0f;
    }
  }
  void
  seek(float second)
  {
    if (sound) {
      seekFreq(static_cast<U32>(second) * sound->samplingRate);
    }
    else {
      seekFreq(static_cast<U32>(second) * DEF_FREQ);
    }
  }
  void
  seekFreq(U32 freq)
  {
    position = static_cast<float>(freq);
  }


 private:
  void
  updatePan()
  {
    double angle = pan * PI_4;
    leftGain = (float)(SQRT2_2 * (cos(angle) - sin(angle)));
    rightGain = (float)(SQRT2_2 * (cos(angle) + sin(angle)));
  }

  Sound* sound;
  float position;
  float startPosition;

  float volume;

  float leftGain;
  float rightGain;
  float pan;
};
}