#pragma once
#include <vector>

#include "SoundMixerChannel.h"

#include "AudioMixerEventGraph.h"

struct FMODSound;

class SoundMixer
{
 public:
  SoundMixer() = default;
  ~SoundMixer() = default;

  SoundMixerChannel*
  addChannel(const std::string& name = "");

  inline SoundMixerChannel*
  getChannel(uint32_t channelIndex)
  {
    return &m_channels[channelIndex];
  }
  inline uint32_t
  getChannelCount()
  {
    return static_cast<uint32_t>(m_channels.size());
  }

  void
  writeSoundData(float* data, int count);

  void
  play();
  void
  pause();
  void
  stop();

  bool
  getIsPlaying();
  bool
  getIsPaused();
  
  float
  getVolume() const { return m_volume; }
  void
  setVolume(float value)
  {
    m_volume = value;
  }

  void
  update(float dt);

 private:
  std::vector<SoundMixerChannel> m_channels;

  AudioMixerEventGraph m_eventGraph;

  bool m_isPlaying = false;
  bool m_isPaused = false;

  float m_volume = 1.0f;
};

