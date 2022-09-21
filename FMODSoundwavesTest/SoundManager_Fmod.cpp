#include "SoundManager_Fmod.h"

#include "fmod.hpp"

#include <string>
#include <cmath>

using namespace std;

SoundManager_Fmod::SoundManager_Fmod():_system(nullptr), _channel(nullptr), _sound(nullptr), _dsp(nullptr), _windowSize(1024)
{
}

int SoundManager_Fmod::initialize()
{
  FMOD_RESULT result = FMOD::System_Create(&_system);
  if (result != FMOD_OK)
  {
    return result;
  }
  else
  {
    _system->init(1, FMOD_INIT_NORMAL, nullptr);

    _system->createDSPByType(FMOD_DSP_TYPE_FFT, &_dsp);
    _dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_TRIANGLE);
    _dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, _windowSize);
  }

  return 0;
}

SoundManager_Fmod::~SoundManager_Fmod()
{
  if (_sound)
  {
    _channel->removeDSP(_dsp);
    _sound->release();
  }

  if (_system)
  {
    _dsp->release();
    _system->close();
    _system->release();
  }
}

int SoundManager_Fmod::loadSoundFromPath(std::string pathToFile)
{
  FMOD_RESULT result = _system->createSound(pathToFile.c_str(), FMOD_LOOP_NORMAL | FMOD_CREATESAMPLE, nullptr, &_sound);
  return result;
}

int SoundManager_Fmod::loadSoundFromMemory(char* memoryPtr, unsigned int memorySize)
{
  FMOD_CREATESOUNDEXINFO sndinfo = { 0 };
  sndinfo.cbsize = sizeof(sndinfo);
  sndinfo.length = memorySize;

  FMOD_RESULT result = _system->createSound(memoryPtr, FMOD_OPENMEMORY | FMOD_LOOP_NORMAL | FMOD_CREATESAMPLE, &sndinfo, &_sound);
  return result;
}

void SoundManager_Fmod::playSound()
{
  _system->playSound(_sound, 0, false, &_channel);
  _channel->addDSP(0, _dsp);
  _dsp->setActive(true);
}

void SoundManager_Fmod::pauseSound(bool unPause)
{
  bool isPaused;
  _channel->getPaused(&isPaused);
  if (isPaused && unPause)
  {
    _channel->setPaused(false);
  }
  else if(!isPaused && !unPause)
  {
    _channel->setPaused(true);
  }
}

void SoundManager_Fmod::update()
{
  _system->update();
}

int SoundManager_Fmod::initializeSpectrum_Linear(int maxBands)
{
  int barSamples = (_windowSize >> 1) / maxBands;

  //calculates num fft samples per bar
  _numSamplesPerBar_linear.clear();
  for (int i = 0; i < maxBands; ++i)
  {
    _numSamplesPerBar_linear.push_back(barSamples);
  }
  return _numSamplesPerBar_linear.size(); //effectiveBars
}

void SoundManager_Fmod::getSpectrum_Linear(float* spectrum)
{
  FMOD_DSP_PARAMETER_FFT* dspFFT = nullptr;
  FMOD_RESULT result = _dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&dspFFT, 0, 0, 0);
  if (dspFFT)
  {
    // Only read / display half of the buffer typically for analysis 
    // as the 2nd half is usually the same data reversed due to the nature of the way FFT works.
    int length = dspFFT->length >> 1;
    int numChannels = dspFFT->numchannels;

    if (length > 0)
    {
      int indexFFT = 0;
      for (int index = 0; index < _numSamplesPerBar_linear.size(); ++index)
      {
        for (int frec = 0; frec < _numSamplesPerBar_linear[index]; ++frec)
        {
          for (int channel = 0; channel < numChannels; ++channel)
          {
            spectrum[index] += dspFFT->spectrum[channel][indexFFT];
          }
          ++indexFFT;
        }
        spectrum[index] /= (float)(_numSamplesPerBar_linear[index] * numChannels);
      }
    }
  }
}

int SoundManager_Fmod::initializeSpectrum_Log(int maxBars)
{
  //calculates octave frequency
  std::vector<int> frequencyOctaves;
  frequencyOctaves.push_back(0);
  for (int i = 1; i < 13; ++i)
  {
    frequencyOctaves.push_back((int)((44100 >> 1) / (float)pow(2, 12 - i)));
  }

  int bandWidth = (44100 / _windowSize);
  int bandsPerOctave = maxBars / 12; //octaves

  //calculates num fft samples per bar
  _numSamplesPerBar_log.clear();
  for (int octave = 0; octave < 12; ++octave)
  {
    int indexLow = frequencyOctaves[octave] / bandWidth;
    int indexHigh = (frequencyOctaves[octave + 1]) / bandWidth;
    int octavaIndexes = (indexHigh - indexLow);

    if (octavaIndexes > 0)
    {
      if (octavaIndexes <= bandsPerOctave)
      {
        for (int count = 0; count < octavaIndexes; ++count)
        {
          _numSamplesPerBar_log.push_back(1);
        }
      }
      else
      {
        for (int count = 0; count < bandsPerOctave; ++count)
        {
          _numSamplesPerBar_log.push_back(octavaIndexes / bandsPerOctave);
        }
      }
    }
  }

  return _numSamplesPerBar_log.size(); //effectiveBars
}

void SoundManager_Fmod::getSpectrum_Log(float* spectrum)
{
  FMOD_DSP_PARAMETER_FFT* dspFFT = nullptr;
  FMOD_RESULT result = _dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&dspFFT, 0, 0, 0);
  if (dspFFT)
  {
    // Only read / display half of the buffer typically for analysis 
    // as the 2nd half is usually the same data reversed due to the nature of the way FFT works.
    int length = dspFFT->length >> 1;
    int numChannels = dspFFT->numchannels;

    if (length > 0)
    {
      int indexFFT = 0;
      for (int index = 0; index < _numSamplesPerBar_log.size(); ++index)
      {
        for (int frec = 0; frec < _numSamplesPerBar_log[index]; ++frec)
        {
          for (int channel = 0; channel < numChannels; ++channel)
          {
            spectrum[index] += dspFFT->spectrum[channel][indexFFT];
          }
          ++indexFFT;
        }
        spectrum[index] /= (float)(_numSamplesPerBar_log[index] * numChannels);
      }
    }
  }
}

void SoundManager_Fmod::getFreq(float* spectrum)
{
  if (!_channel) {
    playSound();
    _channel->setPaused(true);
  }
  _channel->getFrequency(spectrum);
  //FMOD_RESULT result = _dsp->getParameterFloat(FMOD_DSP_FFT_DOMINANT_FREQ, spectrum, 0, 0);
}

unsigned int SoundManager_Fmod::getPCM()
{
  unsigned int pcm = 0;
  _sound->getLength(&pcm, FMOD_TIMEUNIT_PCM);
  return pcm;
}

void SoundManager_Fmod::seekData(unsigned int pcm)
{
  if (!_channel) {
    playSound();
    _channel->setPaused(true);
  }
  _channel->setPosition(pcm, FMOD_TIMEUNIT_PCM);
}
