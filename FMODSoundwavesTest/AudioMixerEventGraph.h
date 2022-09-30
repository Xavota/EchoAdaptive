#pragma once
#include <map>
#include <string>

#include "VariablesSystem.h"

class SoundMixer;

class AudioMixerEventGraph
{
 public:
  AudioMixerEventGraph() = default;
  ~AudioMixerEventGraph() = default;

  virtual void
  eventGraph(SoundMixer* mixer, float dt);

  template<typename T>
  void
  setVariable(const std::string& name, const T& data);
  template<typename T>
  const T&
  getVariable(const std::string& name);

 private:
  std::map<std::string, VariableClass> m_vars;
};

template<typename T>
inline void
AudioMixerEventGraph::setVariable(const std::string& name, const T& data)
{
  if (m_vars.find(name) == m_vars.end()) {
    m_vars[name] = VariableClass();
  }
  m_vars[name].setData<T>(data);
}
template<typename T>
inline const T&
AudioMixerEventGraph::getVariable(const std::string& name)
{
  if (m_vars.find(name) != m_vars.end()) {
    return m_vars[name].getData<T>();
  }
  setVariable<T>(name, T());
  return m_vars[name].getData<T>();
}
