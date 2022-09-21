#pragma once
#include <vector>

#include "SoundMixerChannel.h"

class FMODSound;

class SoundMixer
{
 public:
  SoundMixer() = default;
  ~SoundMixer() = default;

  uint32_t
  addChannel();

  uint32_t
  addChannelTrack(uint32_t channelIndex,
                  FMODSound* track,
                  float startingPoint = 0.0f);

  void
  setChannelPaused(uint32_t channelIndex, bool paused);

  inline SoundMixerChannel*
  getChannel(uint32_t channelIndex)
  {
    return &m_channels[channelIndex];
  }
  inline uint32_t
  getChannelCount()
  {
    return m_channels.size();
  }

  void
  writeSoundData(float* data, int count);

  void
  play();

 private:
  std::vector<SoundMixerChannel> m_channels;
};

