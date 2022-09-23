#pragma once
#include "Prerrequisites.h"

#include <iostream>
#include <vector>
#include <map>

#include "AudioTrack.h"
#include "SoundChannel.h"

class AudioEvent;
class SoundMixer;

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
  addTrack(FMODSound* track, float startingPoint = 0.0f);

  void
  moveTrack(uint32_t index, float startingPoint);

  void
  addEvent(AudioEvent* audioEvent, float fisrtPos, float lastPos = -1.0f);

  void
  setTimePosition(float timePos);
  inline void
  setStartingPosition(float timePos)
  {
    m_startingPosition = timePos;
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

  inline const std::vector<AudioTrack>&
  getAudioTracks() 
  {
    return m_tracks;
  }

  void
  restart();

 private:
  void
  updatePan();


  std::vector<AudioTrack> m_tracks;

  float m_position = 0.0f;
  float m_startingPosition = 0.0f;

  bool m_isPlaying = false;
  bool m_paused = false;

  float m_volume = 1.0f;

  float m_leftGain = SQRT2_2;
  float m_rightGain = SQRT2_2;
  float m_pan = 0.0f;

  std::vector<AudioEvent*> m_events;


  bool m_changePositionRequest = false;
  float m_pendingNewPosition = 0.0f;

  uint32_t m_channelIndex = 0;


  bool m_playOnStart = true;
};

