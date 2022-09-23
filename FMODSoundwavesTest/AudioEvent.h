#pragma once
#include <string>
#include <map>

#include "VariablesSystem.h"

class SoundMixer;
class SoundMixerChannel;

class AudioEvent
{
 public:
  AudioEvent() = default;
  ~AudioEvent() = default;

  virtual void
  triggerEvent(SoundMixer* /*mixer*/, uint32_t /*channelIndex*/) {}

  virtual void
  restart() {}; // TODO: El restart, que reinicie las variables.

  template<typename T>
  void
  setVariable(const std::string& name, const T& data);
  template<typename T>
  const T&
  getVariable(const std::string& name);

  inline float
  getFirstTime()
  {
    return m_startTime;
  }
  inline float
  getLastTime()
  {
    return m_finalTime;
  }
  inline void
  setFirstTime(float firstTime)
  {
    m_startTime = firstTime;;
  }
  inline void
  setLastTime(float lastTime)
  {
    m_finalTime = lastTime;
  }

protected:
  std::map<std::string, VariableClass> m_vars;

  float m_startTime = 0.0f;
  float m_finalTime = 0.0f;
};

template<typename T>
inline void
AudioEvent::setVariable(const std::string& name, const T& data)
{
  if (m_vars.find(name) == m_vars.end()) {
    m_vars[name] = VariableClass();
  }
  m_vars[name].setData<T>(data);
}
template<typename T>
inline const T&
AudioEvent::getVariable(const std::string& name)
{
  if (m_vars.find(name) != m_vars.end()) {
    return m_vars[name].getData<T>();
  }
  return T();
}