#pragma once
#include <vector>

#include "Prerrequisites.h"

#include "SoundChannel.h"

class SoundEffect;

class SoundChannelGroup
{
 public:
   SoundChannelGroup() :
    paused(false),
    volume(1.0f),
    pitch(1.0f)
  {
    SetPan(0.0f);
  }
  ~SoundChannelGroup()
  {
    m_childGroups.clear();
    m_childChannels.clear();
    m_effects.clear();
  }

  void WriteSoundData(float* data, int count, float parentPitch);

  bool GetPaused() const { return paused; }
  void SetPaused(bool value) { paused = value; }

  void SetLoopAll(bool value)
  {
    for (auto& cg : m_childGroups) {
      if (cg) cg->SetLoopAll(value);
    }
    for (auto& cc : m_childChannels) {
      if (cc) cc->SetLoop(value);
    }
  }

  float GetVolume() const { return volume; }
  void SetVolume(float value)
  {
    volume = value;
  }

  float GetPitch() const { return pitch; }
  void SetPitch(float value)
  {
    if (value < 0.25f) pitch = 0.25f;
    else if (value > 4.0f) pitch = 4.0f;
    else pitch = value;
  }

  float GetPan() const { return pan; }
  void SetPan(float value)
  {
    if (value < -1.0f) pan = -1.0f;
    else if (value > 1.0f) pan = 1.0f;
    else pan = value;
    UpdatePan();
  }

  void
  AddChannelGroup(SoundChannelGroup* newChannelGroup)
  {
    m_childGroups.emplace_back(newChannelGroup);
  }
  void
  AddChannel(SoundChannel* newChannel)
  {
    m_childChannels.emplace_back(newChannel);
  }

  void
  addSoundEffect(SoundEffect* sfx)
  {
    m_effects.emplace_back(sfx);
  }


  inline std::vector<SoundChannelGroup*>&
  getChannelGroupChildren()
  {
    return m_childGroups;
  }
  inline std::vector<SoundChannel*>&
  getChannelChildren()
  {
    return m_childChannels;
  }

 private:
  void UpdatePan()
  {
    double angle = pan * PI_4;
    leftGain = (float)(SQRT2_2 * (cos(angle) - sin(angle)));
    rightGain = (float)(SQRT2_2 * (cos(angle) + sin(angle)));
  }

  std::vector<SoundChannelGroup*> m_childGroups;
  std::vector<SoundChannel*> m_childChannels;

  bool paused;

  float volume;

  float pitch;

  float leftGain;
  float rightGain;
  float pan;

  std::vector<SoundEffect*> m_effects;
};