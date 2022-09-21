#pragma once

class SoundMixer;
class SoundMixerChannel;

class AudioImGuiImpl
{
 public:
  AudioImGuiImpl() = default;
  ~AudioImGuiImpl() = default;

  void
  drawUI(SoundMixer* mixerTool, int& pushID);
};

