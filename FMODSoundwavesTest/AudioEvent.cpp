#include "AudioEvent.h"

void
AudioEvent::setBoolVariable(std::string varName, bool newValue)
{
  m_boolVariables[varName] = newValue;
}

void
AudioEvent::setIntVariable(std::string varName, int newValue)
{
  m_intVariables[varName] = newValue;
}

void
AudioEvent::setFloatVariable(std::string varName, float newValue)
{
  m_floatVariables[varName] = newValue;
}

void
AudioEvent::setStringVariable(std::string varName, std::string newValue)
{
  m_stringVariables[varName] = newValue;
}

void
AudioEvent::setEventVariable(std::string varName, AudioEvent* newValue)
{
  m_eventVariables[varName] = newValue;
}
