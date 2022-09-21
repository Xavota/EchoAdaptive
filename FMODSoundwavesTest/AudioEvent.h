#pragma once
#include <string>
#include <map>

class SoundMixer;
class SoundMixerChannel;

class AudioEvent
{
 public:
  AudioEvent() = default;
  ~AudioEvent() = default;

  virtual void
  triggerEvent(SoundMixer* /*mixer*/, uint32_t /*channelIndex*/) {}

  void
  setBoolVariable(std::string varName, bool newValue);

  void
  setIntVariable(std::string varName, int newValue);

  void
  setFloatVariable(std::string varName, float newValue);

  void
  setStringVariable(std::string varName, std::string newValue);

  void
  setEventVariable(std::string varName, AudioEvent* newValue);

 protected:
  std::map<std::string, bool> m_boolVariables;
  std::map<std::string, int> m_intVariables;
  std::map<std::string, float> m_floatVariables;
  std::map<std::string, std::string> m_stringVariables;
  std::map<std::string, AudioEvent*> m_eventVariables;
};

