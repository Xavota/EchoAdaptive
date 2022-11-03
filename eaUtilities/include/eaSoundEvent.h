#pragma once
#include "eaPrerrequisitesUtilities.h"

#include "eaVariableClass.h"

namespace eaSdkEngine {
class SoundMixer;

class SoundEvent
{
 public:
  SoundEvent() = default;
  ~SoundEvent() = default;

  virtual void
  triggerEvent(SoundMixer* /*mixer*/, uint32_t /*channelIndex*/) {}

  virtual void
  restart() {}; // TODO: El restart, que reinicie las variables.

  template<typename T>
  void
  setVariable(const String& name, const T& data);
  template<typename T>
  const T&
  getVariable(const String& name);

  inline float
  getFirstTime() const
  {
    return m_startTime;
  }
  inline float
  getLastTime() const
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
  Map<String, VariableClass> m_vars;

  float m_startTime = 0.0f;
  float m_finalTime = 0.0f;
};

template<typename T>
inline void
SoundEvent::setVariable(const String& name, const T& data)
{
  if (m_vars.find(name) == m_vars.end()) {
    m_vars[name] = VariableClass();
  }
  m_vars[name].setData<T>(data);
}
template<typename T>
inline const T&
SoundEvent::getVariable(const String& name)
{
  if (m_vars.find(name) != m_vars.end()) {
    return m_vars[name].getData<T>();
  }
  setVariable<T>(name, T());
  return m_vars[name].getData<T>();
}
}