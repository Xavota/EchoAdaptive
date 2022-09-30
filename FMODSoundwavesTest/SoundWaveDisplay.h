#pragma once
#include <SFML/Graphics.hpp>

#include <vector>

struct FMODSound;

class SoundWaveDisplay
{
public:
  SoundWaveDisplay() = default;
  SoundWaveDisplay(sf::Vector2u screenSize);
  ~SoundWaveDisplay() = default;

  void
  setScreenSize(sf::Vector2u screenSize);

  void
  addSoundToDisplay(FMODSound* sound);
  
  void
  scroll(int scrll);

  void
  zoom(int scrll);

  void
  expand(int scrll);

  void
  moveHorizontally(float movement);

  void
  render(sf::RenderWindow* window);

private:
  float m_leftSecondStamp = 0.0f;
  float m_secondsInDisplay = 15.0f;
  int m_sampleResolution = 260;
  const int m_samplesPerPixel = 10;
  float m_waveDX = 0.5f;
  float m_horizontalScreenSize = 0.0f;

  float m_scrollbarSpeed = 50.0f;
  float m_scrollbarUpPixels = 0.0f;
  float m_scrollbarVerticalScreenSize = 0.0f;
  float m_scrollbarVerticalContentSize = 0.0f;
  float m_scrollbarVerticalOutSpace = 0.0f;
  float m_scrollbarSize = 0.0f;
  float m_verticalSizeOfChannels = 200.0f;
  float m_verticalSpaceBetweenChannels = 50.0f;

  std::vector<FMODSound*> m_soundsInDisplay;


  float m_secScreenxRatio = 5.0f / 1280.0f;




  sf::RectangleShape m_shape;
};