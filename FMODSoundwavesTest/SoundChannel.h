#pragma once
#include <vector>

#include "FMODSound.h"

class SoundEffect;
class SynthetiseSound;

class SoundChannel
{
public:
  SoundChannel() :
  sound(nullptr),
  syntheticSound(nullptr),
  position(0.0f),
  paused(false),
  loop(false),
  volume(1.0f),
  pitch(1.0f)
  {
    SetPan(0.0f);
  }
  ~SoundChannel()
  {
    m_effects.clear();
    sound = nullptr;
  }
  void
  PlaySound(FMODSound* mySound, float _paused = false);
  void
  PlaySyntheticSound(SynthetiseSound* sound);
  void
  Stop();
  void
  WriteSoundData(float* data, int count, float parentPitch);

  bool
  GetPaused() const { return paused; }
  void
  SetPaused(bool value) { paused = value; }

  bool
  GetLoop() const { return loop; }
  void
  SetLoop(bool value) { loop = value; }

  float
  GetVolume() const { return volume; }
  void
  SetVolume(float value)
  {
    volume = value;
  }

  float
  GetPitch() const { return pitch; }
  void
  SetPitch(float value)
  {
    if (value < 0.25f) pitch = 0.25f;
    else if (value > 4.0f) pitch = 4.0f;
    else pitch = value;
  }

  float
  GetPan() const { return pan; }
  void
  SetPan(float value)
  {
    if (value < -1.0f) pan = -1.0f;
    else if (value > 1.0f) pan = 1.0f;
    else pan = value;
    UpdatePan();
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

  void
  addSoundEffect(SoundEffect* sfx)
  {
    m_effects.emplace_back(sfx);
  }

 private:
  void
  UpdatePan()
  {
    double angle = pan * PI_4;
    leftGain = (float)(SQRT2_2 * (cos(angle) - sin(angle)));
    rightGain = (float)(SQRT2_2 * (cos(angle) + sin(angle)));
  }

  FMODSound* sound;
  float position;

  bool paused;

  bool loop;

  float volume;

  float pitch;

  float leftGain;
  float rightGain;
  float pan;

  SynthetiseSound* syntheticSound;

  std::vector<SoundEffect*> m_effects;
};

