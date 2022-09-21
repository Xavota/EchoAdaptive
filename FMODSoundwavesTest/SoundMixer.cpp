#include "SoundMixer.h"

uint32_t
SoundMixer::addChannel()
{
  uint32_t index = m_channels.size();
  m_channels.emplace_back(SoundMixerChannel());
  m_channels[index].setChannelIndex(index);
  return index;
}

uint32_t
SoundMixer::addChannelTrack(uint32_t channelIndex,
                            FMODSound* sound,
                            float startingPoint)
{
  return m_channels[channelIndex].addTrack(sound, startingPoint);
}

void
SoundMixer::setChannelPaused(uint32_t channelIndex, bool paused)
{
  m_channels[channelIndex].setPaused(paused);
}

void
SoundMixer::writeSoundData(float* data, int count)
{
  for (auto& c : m_channels) {
    c.writeSoundData(this, data, count);
  }
}

void
SoundMixer::play()
{
  for (auto& c : m_channels) {
    if (c.getPlayOnStart()) {
      c.setPaused(false);
    }
  }
}
