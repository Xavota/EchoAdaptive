#pragma once
#include "Prerrequisites.h"

#include <iostream>

#include "FMODSound.h"

class AudioTrack
{
public:
  AudioTrack() :
  sound(nullptr),
  position(0.0f),
  volume(1.0f)
  {
    setPan(0.0f);
  }
  ~AudioTrack()
  {
    sound = nullptr;
  }
  void
  setSound(FMODSound* mySound)
  {
    sound = mySound;
  }
  inline const FMODSound*
  getSound() const
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

  FMODSound* sound;
  float position;
  float startPosition;

  float volume;

  float leftGain;
  float rightGain;
  float pan;
};