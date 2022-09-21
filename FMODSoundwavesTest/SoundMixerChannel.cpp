#include "SoundMixerChannel.h"

#include "AudioEvent.h"

uint32_t
SoundMixerChannel::addTrack(FMODSound* track, float startingPoint)
{
  uint32_t index = m_tracks.size();

  m_tracks.emplace_back(AudioTrack());
  m_tracks[index].setSound(track);
  m_tracks[index].setStartPosition(startingPoint * DEF_FREQ);
  
  return index;
}

void
SoundMixerChannel::moveTrack(uint32_t index, float startingPoint)
{
  m_tracks[index].setStartPosition(startingPoint * DEF_FREQ);
}

void
SoundMixerChannel::addEvent(float position, AudioEvent* audioEvent)
{
  m_events[position * DEF_FREQ] = audioEvent;
}

void
SoundMixerChannel::setTimePosition(float timePos)
{
  m_changePositionRequest = true;
  m_pendingNewPosition = timePos * DEF_FREQ;
}

void SoundMixerChannel::setPaused(bool _paused)
{
  m_paused = _paused;
}

void
SoundMixerChannel::writeSoundData(SoundMixer* mixer, float* data, int count)
{
  if (m_paused) return;

  if (m_changePositionRequest) {
    m_position = m_pendingNewPosition;
    m_changePositionRequest = false;
  }

  for (U32 i = 0; i < count; i += 2) {
    auto truePos = static_cast<U32>(m_position);
    //std::cout << truePos << std::endl;
    for (auto& t : m_tracks) {
      if (truePos >= t.getStartPosition()
       && truePos < t.getStartPosition() + t.getMaxPositionFreq()) {
        t.seekFreq(truePos);
        float* newData = new float[count];
        memset(newData, 0, count * sizeof(float));
        t.writeSoundData(newData, i);

        data[i] += newData[i] * m_volume * m_leftGain;
        data[i + 1] += newData[i + 1] * m_volume * m_rightGain;

        delete[] newData;
      }
    }

    for (auto& e : m_events) {
      if (truePos == static_cast<U32>(e.first)) {
        e.second->triggerEvent(mixer, m_channelIndex);
      }
    }
    m_position += 1;
    if (m_changePositionRequest) {
      m_position = m_pendingNewPosition;
      m_changePositionRequest = false;
    }
  }
}


void
SoundMixerChannel::updatePan()
{
  double angle = m_pan * PI_4;
  m_leftGain = (float)(SQRT2_2 * (cos(angle) - sin(angle)));
  m_rightGain = (float)(SQRT2_2 * (cos(angle) + sin(angle)));
}