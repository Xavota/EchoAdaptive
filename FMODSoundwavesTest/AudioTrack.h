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
  inline const FMODSound const*
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
  getStartPosition()
  {
    return startPosition;
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
  getPositionSec()
  {
    if (sound) {
      return position / sound->samplingRate;
    }
    else {
      return position / DEF_FREQ;
    }
  }
  float
  getPositionFreq()
  {
    return position;
  }
  float
  getMaxPositionSec()
  {
    if (sound) {
      return sound->count / (sound->samplingRate * sound->numChannels);
    }
    else {
      return 0;
    }
  }
  float
  getMaxPositionFreq()
  {
    if (sound) {
      return sound->count / sound->numChannels;
    }
    else {
      return 0;
    }
  }
  void
  seek(float second)
  {
    if (sound) {
      seekFreq(second * sound->samplingRate);
    }
    else {
      seekFreq(second * DEF_FREQ);
    }
  }
  void
  seekFreq(U32 freq)
  {
    position = freq;
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