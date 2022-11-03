#pragma once
#include "eaPrerrequisitesUtilities.h"

#include "eaVariableClass.h"

namespace eaSdkEngine {
class SoundMixer;

class SoundMixerEventGraph
{
 public:
  SoundMixerEventGraph() = default;
  ~SoundMixerEventGraph() = default;

  virtual void
  eventGraph(SoundMixer* mixer, float dt);

  template<typename T>
  void
  setVariable(const String& name, const T& data);
  template<typename T>
  const T&
  getVariable(const String& name);

 private:
  Map<String, VariableClass> m_vars;
};

template<typename T>
inline void
SoundMixerEventGraph::setVariable(const String& name, const T& data)
{
  if (m_vars.find(name) == m_vars.end()) {
    m_vars[name] = VariableClass();
  }
  m_vars[name].setData<T>(data);
}
template<typename T>
inline const T&
SoundMixerEventGraph::getVariable(const String& name)
{
  if (m_vars.find(name) != m_vars.end()) {
    return m_vars[name].getData<T>();
  }
  setVariable<T>(name, T());
  return m_vars[name].getData<T>();
}
}