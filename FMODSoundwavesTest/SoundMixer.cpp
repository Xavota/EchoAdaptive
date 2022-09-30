#include "SoundMixer.h"

SoundMixerChannel*
SoundMixer::addChannel()
{
  auto index = static_cast<U32>(m_channels.size());
  m_channels.emplace_back(SoundMixerChannel());
  m_channels[index].setChannelIndex(index);
  return &m_channels[index];
}

void
SoundMixer::writeSoundData(float* data, int count)
{
  if (m_isPaused || !m_isPlaying) return;

  for (auto& c : m_channels) {
    c.writeSoundData(this, data, count);
  }

  for (int i = 0; i < count; ++i) {
    data[i] *= m_volume;
  }
}

void
SoundMixer::play()
{
  if (!m_isPlaying) {
    for (auto& c : m_channels) {
      c.start();
    }
  }
  else {
    for (auto& c : m_channels) {
      if (c.getIsPlaying()) {
        c.play();
      }
    }
  }

  m_isPlaying = true;
  m_isPaused = false;
}

void
SoundMixer::pause()
{
  m_isPaused = true;

  for (auto& c : m_channels) {
    c.pause();
  }
}

void
SoundMixer::stop()
{
  m_isPlaying = false;
  m_isPaused = true;

  for (auto& c : m_channels) {
    c.restart();
  }
}

bool
SoundMixer::getIsPlaying()
{
  return m_isPlaying;
}

bool
SoundMixer::getIsPaused()
{
  return m_isPaused;
}

void
SoundMixer::update(float dt)
{
  if (!m_isPlaying || m_isPaused) {
    m_eventGraph.eventGraph(this, dt);
  }
}
