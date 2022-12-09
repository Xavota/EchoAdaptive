#include "eaSoundMixerChannel.h"

#include "eaSoundEvent.h"

namespace eaSdkEngine {
uint32_t
SoundMixerChannel::addTrack(Sound* track, float startingPoint)
{
  auto index = static_cast<U32>(m_tracks.size());

  m_tracks.emplace_back(SoundTrack());
  m_tracks[index].setSound(track);
  m_tracks[index].setStartPosition(startingPoint);
  
  return index;
}
void
SoundMixerChannel::removeTrack(uint32 index)
{
  if (index < m_tracks.size())
  {
    m_tracks.erase(m_tracks.begin() + index); // TODO: Checar si el audio que tiene el track sólo lo tiene él para eliminarlo.
  }
}
void
SoundMixerChannel::moveTrack(uint32_t index, float startingPoint)
{
  m_tracks[index].setStartPosition(startingPoint * DEF_FREQ);
}
void
SoundMixerChannel::moveTrackPCM(uint32_t index, float startingPointPCM)
{
  m_tracks[index].setStartPosition(startingPointPCM);
}
void
SoundMixerChannel::addEvent(SoundEvent* audioEvent,
                            float fisrtPos,
                            float lastPos)
{
  if (lastPos < 0.0f) {
    lastPos = fisrtPos;
  }

  m_events.push_back(audioEvent);
  audioEvent->setFirstTime(fisrtPos);
  audioEvent->setLastTime(lastPos);
}
float
SoundMixerChannel::getTimePositionFreq()
{
  return m_position;
}
float
SoundMixerChannel::getTimePositionSec()
{
  return m_position / DEF_FREQ;
}
void
SoundMixerChannel::setTimePositionFreq(float timePos)
{
  m_changePositionRequest = true;
  m_pendingNewPosition = timePos;
}
void
SoundMixerChannel::setTimePositionSec(float timePos)
{
  m_changePositionRequest = true;
  m_pendingNewPosition = timePos * DEF_FREQ;
}
void
SoundMixerChannel::start()
{
  if (m_playOnStart) {
    play();
  }
}
void
SoundMixerChannel::play()
{
  m_isPlaying = true;
  m_paused = false;
}
void
SoundMixerChannel::pause()
{
  m_paused = true;
}
void
SoundMixerChannel::restart()
{
  m_position = m_startingPosition;
  m_paused = true;
  m_isPlaying = false;

  for (auto& e : m_events) {
    e->restart();
  }
}
void
SoundMixerChannel::updatePan()
{
  double angle = m_pan * PI_4;
  m_leftGain = (float)(SQRT2_2 * (cos(angle) - sin(angle)));
  m_rightGain = (float)(SQRT2_2 * (cos(angle) + sin(angle)));
}

void
SoundMixerChannel::writeSoundData(SoundMixer* mixer, float* data, int count)
{
  if (m_paused || !m_isPlaying) return;

  if (m_changePositionRequest) {
    m_position = m_pendingNewPosition;
    m_changePositionRequest = false;
  }

  for (int i = 0; i < count; i += 2) {
    auto truePos = static_cast<U32>(m_position);
    for (auto& t : m_tracks) {
      float startPos = t.getStartPosition();
      if (truePos >= startPos
       && truePos < startPos + t.getMaxPositionFreq()) {
        t.seekFreq(truePos);
        //float* newData = new float[count];
        //memset(newData, 0, count * sizeof(float));
        //t.writeSoundData(newData, i);
        //
        //data[i] += newData[i] * m_volume * m_leftGain;
        //data[i + 1] += newData[i + 1] * m_volume * m_rightGain;
        //
        //delete[] newData;

        float ldata, rdata;
        t.getSoundData(ldata, rdata);

        data[i] += ldata * m_volume * m_leftGain;
        data[i + 1] += rdata * m_volume * m_rightGain;
      }
    }

    for (auto& e : m_events) {
      if (truePos >= static_cast<U32>(e->getFirstTime())
       && truePos <= static_cast<U32>(e->getLastTime())) {
        e->triggerEvent(mixer, m_channelIndex);
      }
    }
    m_position += 1;
    if (m_changePositionRequest) {
      m_position = m_pendingNewPosition;
      m_changePositionRequest = false;
    }
  }
}
}