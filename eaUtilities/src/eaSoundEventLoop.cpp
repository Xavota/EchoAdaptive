#include "eaSoundEventLoop.h"

#include "eaSoundMixer.h"
#include "eaSoundMixerChannel.h"

namespace eaSdkEngine {
void
SoundEventLoop::triggerEvent(SoundMixer* mixer, uint32_t channelIndex)
{
  mixer->getChannel(channelIndex)->setTimePositionFreq(
                                           getVariable<float>("StartLoopTime"));
}

}