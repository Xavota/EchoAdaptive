#pragma once
#include "AudioEvent.h"

class AudioEventLoop : public AudioEvent
{
 public:
  AudioEventLoop() = default;
  ~AudioEventLoop() = default;
  
  void
  triggerEvent(SoundMixer* mixer, uint32_t channelIndex) override;

 private:
  
};

