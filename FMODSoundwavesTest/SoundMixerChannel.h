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
  addEvent(float position, AudioEvent* audioEvent);

  void
  setTimePosition(float timePos);

  void
  setPaused(bool _paused);

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

 private:
  void
  updatePan();


  std::vector<AudioTrack> m_tracks;

  float m_position = 0.0f;

  bool m_paused = false;

  float m_volume = 1.0f;

  float m_leftGain = SQRT2_2;
  float m_rightGain = SQRT2_2;
  float m_pan = 0.0f;

  std::map<float, AudioEvent*> m_events;


  bool m_changePositionRequest = false;
  float m_pendingNewPosition = 0.0f;

  uint32_t m_channelIndex = 0;


  bool m_playOnStart = true;
};

