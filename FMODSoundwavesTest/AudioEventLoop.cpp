#include "AudioEventLoop.h"

#include "SoundMixer.h"
#include "SoundMixerChannel.h"

void
AudioEventLoop::triggerEvent(SoundMixer* mixer, uint32_t channelIndex)
{
  mixer->getChannel(channelIndex)->setTimePosition(
   m_floatVariables["StartLoopTime"]);
}
