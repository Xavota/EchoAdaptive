#include "eaSoundMixerEventGraph.h"

#include "eaSoundMixer.h"

namespace eaSdkEngine {
void
SoundMixerEventGraph::eventGraph(SoundMixer* mixer, float dt)
{
  float time = getVariable<float>("Time");
  mixer->setVolume(sin(time));
  setVariable<float>("Time", time + dt);
}
}