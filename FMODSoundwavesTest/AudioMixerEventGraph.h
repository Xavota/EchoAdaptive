#pragma once
class SoundMixer;

class AudioMixerEventGraph
{
 public:
  AudioMixerEventGraph();
  ~AudioMixerEventGraph();

  virtual void
  eventGraph(SoundMixer* mixer);

 private:
  
};

