#include "SoundWaveDisplay.h"
#include "Prerrequisites.h"

#include "FMODSound.h"

SoundWaveDisplay::SoundWaveDisplay(sf::Vector2u screenSize) :
  m_leftSecondStamp(0.0f),
  m_samplesPerPixel(2),
  m_scrollbarUpPixels(0.0f)
{
  m_secondsInDisplay = screenSize.x * m_secScreenxRatio;
  setScreenSize(screenSize);


  m_shape.setFillColor(sf::Color::Green);
}

void
SoundWaveDisplay::setScreenSize(sf::Vector2u screenSize)
{
  m_horizontalScreenSize = static_cast<float>(screenSize.x);
  m_sampleResolution =
  static_cast<int>(DEF_FREQ
                 / ((m_horizontalScreenSize * m_samplesPerPixel)
                   / m_secondsInDisplay));
  m_sampleResolution = m_sampleResolution >= 1 ? m_sampleResolution : 1;
  m_waveDX = m_horizontalScreenSize
           / ((DEF_FREQ * m_secondsInDisplay) / m_sampleResolution);

  m_scrollbarVerticalScreenSize = static_cast<float>(screenSize.y);
  m_scrollbarVerticalContentSize = m_soundsInDisplay.size()
                                 * (m_verticalSizeOfChannels
                                  + m_verticalSpaceBetweenChannels);
  m_scrollbarVerticalOutSpace = m_scrollbarVerticalContentSize
                              - m_scrollbarVerticalScreenSize;
  m_scrollbarVerticalOutSpace = m_scrollbarVerticalOutSpace > 0.0f
                              ? m_scrollbarVerticalOutSpace : 0.0f;
}

void
SoundWaveDisplay::addSoundToDisplay(FMODSound* sound)
{
  m_soundsInDisplay.push_back(sound);

  m_scrollbarVerticalContentSize = m_soundsInDisplay.size()
                                 * (m_verticalSizeOfChannels
                                  + m_verticalSpaceBetweenChannels);
  m_scrollbarVerticalOutSpace = m_scrollbarVerticalContentSize
                              - m_scrollbarVerticalScreenSize;
  m_scrollbarVerticalOutSpace = m_scrollbarVerticalOutSpace > 0.0f
                              ? m_scrollbarVerticalOutSpace : 0.0f;
}

void
SoundWaveDisplay::scroll(int scrll)
{
  m_scrollbarUpPixels -= m_scrollbarSpeed * -scrll;
  m_scrollbarUpPixels = m_scrollbarUpPixels >= 0 ? m_scrollbarUpPixels : 0;
  m_scrollbarUpPixels = m_scrollbarUpPixels < m_scrollbarVerticalOutSpace ? m_scrollbarUpPixels : m_scrollbarVerticalOutSpace;
}

void
SoundWaveDisplay::zoom(int scrll)
{
  m_secondsInDisplay *= static_cast<float>(pow(0.25f, scrll));
  m_sampleResolution =
  static_cast<int>(DEF_FREQ
                 / ((m_horizontalScreenSize * m_samplesPerPixel)
                   / m_secondsInDisplay));
  m_sampleResolution = m_sampleResolution >= 1 ? m_sampleResolution : 1;
  m_waveDX = m_horizontalScreenSize
           / ((DEF_FREQ * m_secondsInDisplay) / m_sampleResolution);
}

void
SoundWaveDisplay::expand(int scrll)
{
  m_verticalSizeOfChannels += 25.0f * scrll;
}

void
SoundWaveDisplay::moveHorizontally(float movement)
{
  float samplesWidth = m_horizontalScreenSize / m_waveDX;
  m_leftSecondStamp += (movement * m_samplesPerPixel) / (samplesWidth / m_secondsInDisplay);
  m_leftSecondStamp = m_leftSecondStamp >= 0 ? m_leftSecondStamp : 0;
}

void
SoundWaveDisplay::render(sf::RenderWindow* window)
{
  if (!window) return;

  static float height = 0.0f;
  static sf::Vector2f newPos{ 0.0f, 0.0f };
  static sf::Vector2f dir{ 0.0f, 0.0f };
  static float dist = 0.0f;
  static float angle = 0.0f;
  static sf::Vector2f shapeLastPos;

  int soundsCount = static_cast<int>(m_soundsInDisplay.size());


  int maxSamplesWidth = static_cast<int>(m_horizontalScreenSize / m_waveDX) + 1;

  for (int i = 0; i < soundsCount; ++i) {
    if (!m_soundsInDisplay[i] || !m_soundsInDisplay[i]->data) continue;

    int numChanns = m_soundsInDisplay[i]->numChannels == 1
                  ? 1
                  : (m_soundsInDisplay[i]->numChannels >= 1
                   ? 2
                   : 0);
    if (numChanns > 0) {
      float maxChannelSize = m_soundsInDisplay[i]->numChannels == 1
                           ? m_verticalSizeOfChannels
                           : (m_soundsInDisplay[i]->numChannels > 1
                            ? m_verticalSizeOfChannels * 0.5f - 5.0f
                            : 0);
    

      float* soundData = m_soundsInDisplay[i]->data;
      float midChannelHeight = m_scrollbarUpPixels
                             + (m_verticalSpaceBetweenChannels
                             + maxChannelSize) * 0.5f
                             + (m_verticalSpaceBetweenChannels + maxChannelSize) * i;
      shapeLastPos = { 0.0f, midChannelHeight - soundData[0] * maxChannelSize * 0.5f };
      for (int j = m_sampleResolution * numChanns, k = 1;
           j < static_cast<int>(m_soundsInDisplay[i]->count) && k < maxSamplesWidth;
           j += m_sampleResolution * numChanns, ++k) {
        height = midChannelHeight - soundData[j] * maxChannelSize * 0.5f;
        newPos = { shapeLastPos.x + m_waveDX, height };
        dir = newPos - shapeLastPos;
        dist = static_cast<float>(pow(pow(dir.x, 2) + pow(dir.y, 2), 0.5f));
        m_shape.setSize({ 1.0f, dist });
        angle = atan2(dir.y, dir.x) * 360 / 3.14159265f;
        m_shape.setRotation(angle);
        m_shape.setPosition((newPos + shapeLastPos) * 0.5f);
        window->draw(m_shape);
        shapeLastPos = newPos;
      }
    
      if (numChanns == 2) {
        midChannelHeight = m_scrollbarUpPixels
                         + m_verticalSpaceBetweenChannels * 0.5f
                         + maxChannelSize * 1.5f + 10.0f
                         + (m_verticalSpaceBetweenChannels + maxChannelSize) * i;
        shapeLastPos = { 0.0f, midChannelHeight - soundData[0] * maxChannelSize * 0.5f };
        for (int j = m_sampleResolution * numChanns, k = 1;
             j < static_cast<int>(m_soundsInDisplay[i]->count) && k < maxSamplesWidth;
             j += m_sampleResolution * numChanns, ++k) {
          height = midChannelHeight - soundData[j + 1] * maxChannelSize * 0.5f;
          newPos = { shapeLastPos.x + m_waveDX, height };
          dir = newPos - shapeLastPos;
          dist = static_cast<float>(pow(pow(dir.x, 2) + pow(dir.y, 2), 0.5f));
          m_shape.setSize({ 1.0f, dist });
          angle = atan2(dir.y, dir.x) * 360 / 3.14159265f;
          m_shape.setRotation(angle);
          m_shape.setPosition((newPos + shapeLastPos) * 0.5f);
          window->draw(m_shape);
          shapeLastPos = newPos;
        }
      }
    }
  }
}