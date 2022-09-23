#include "AudioMixerEventGraph.h"

#include "SoundMixer.h"

void
AudioMixerEventGraph::eventGraph(SoundMixer* mixer, float dt)
{
  float time = getVariable<float>("Time");
  mixer->setVolume(sin(time));
  setVariable<float>("Time", time + dt);
}
