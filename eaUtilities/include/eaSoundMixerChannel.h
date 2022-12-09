#pragma once
#include "eaPrerrequisitesUtilities.h"

#include "eaSoundTrack.h"

namespace eaSdkEngine {
class SoundEvent;
class SoundMixer;

struct Sound;

class SoundMixerChannel
{
 public:
  SoundMixerChannel() = default;
  ~SoundMixerChannel() = default;

  void
  setChannelIndex(uint32_t index)
  {
    m_channelIndex = index;
  }

  uint32_t
  addTrack(Sound* track, float startingPoint = 0.0f);
  void
  removeTrack(uint32 index);

  void
  moveTrack(uint32_t index, float startingPoint);
  void
  moveTrackPCM(uint32_t index, float startingPointPCM);

  void
  addEvent(SoundEvent* audioEvent, float fisrtPos, float lastPos = -1.0f);
  inline const Vector<SoundEvent*>&
  getEvents()
  {
    return m_events;
  }

  float
  getTimePositionFreq();
  float
  getTimePositionSec();
  inline float
  getStartingPositionFreq()
  {
    return m_startingPosition;
  }
  inline float
  getStartingPositionSec()
  {
    return m_startingPosition / DEF_FREQ;
  }
  void
  setTimePositionFreq(float timePos);
  void
  setTimePositionSec(float timePos);
  inline void
  setStartingPositionFreq(float timePos)
  {
    m_startingPosition = timePos;
  }
  inline void
  setStartingPositionSec(float timePos)
  {
    m_startingPosition = timePos * DEF_FREQ;
  }
  
  void
  start();
  void
  play();
  void
  pause();

  inline bool
  getIsPlaying()
  {
    return m_isPlaying;
  }
  inline bool
  getIsPaused()
  {
    return m_paused;
  }

  inline bool
  getPlayOnStart()
  {
    return m_playOnStart;
  }
  inline void
  setPlayOnStart(bool playOn)
  {
    m_playOnStart = playOn;
  }

  void
  writeSoundData(SoundMixer* mixer, float* data, int count);

  float
  getVolume() const { return m_volume; }
  void
  setVolume(float value)
  {
    m_volume = value;
  }

  float
  getPan() const { return m_pan; }
  void
  setPan(float value)
  {
    if (value < -1.0f) m_pan = -1.0f;
    else if (value > 1.0f) m_pan = 1.0f;
    else m_pan = value;
    updatePan();
  }

  inline const Vector<SoundTrack>&
  getAudioTracks() 
  {
    return m_tracks;
  }

  void
  restart();

  inline void
  setName(const String& newName)
  {
    m_name = newName;
  }
  inline const String&
  getName() const
  {
    return m_name;
  }

 private:
  void
  updatePan();


  Vector<SoundTrack> m_tracks;

  float m_position = 0.0f;
  float m_startingPosition = 0.0f;

  bool m_isPlaying = false;
  bool m_paused = false;

  float m_volume = 1.0f;

  float m_leftGain = SQRT2_2;
  float m_rightGain = SQRT2_2;
  float m_pan = 0.0f;

  Vector<SoundEvent*> m_events;


  bool m_changePositionRequest = false;
  float m_pendingNewPosition = 0.0f;

  uint32_t m_channelIndex = 0;


  bool m_playOnStart = true;

  String m_name;
};
}