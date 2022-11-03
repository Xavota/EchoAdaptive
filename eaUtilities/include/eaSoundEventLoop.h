#pragma once
#include "eaPrerrequisitesUtilities.h"

#include "eaSoundEvent.h"

namespace eaSdkEngine {
class SoundMixer;

class SoundEventLoop : public SoundEvent
{
 public:
  SoundEventLoop() = default;
  ~SoundEventLoop() = default;

  void
  triggerEvent(SoundMixer* mixer, uint32_t channelIndex) override;

 private:

};
}