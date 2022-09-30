#pragma once

#include <SFML/Graphics.hpp>

class SoundMixer;
class SoundMixerChannel;

class AudioImGuiImpl
{
 public:
  AudioImGuiImpl();
  ~AudioImGuiImpl() = default;

  void
  drawUI(sf::RenderWindow* window, SoundMixer* mixerTool, int& pushID);
};

