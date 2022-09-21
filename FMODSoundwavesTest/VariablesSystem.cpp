#include "VariablesSystem.h"

void
VariableClass::setType(eVAR_TYPE::E type)
{
  m_type = type;

  m_data.clear();

  int m_size = 0;
  switch (type)
  {
  case eVAR_TYPE::kInt:
    m_size = sizeof(int);
    break;
  case eVAR_TYPE::kShortInt:
    m_size = sizeof(short int);
    break;
  case eVAR_TYPE::kLongInt:
    m_size = sizeof(long int);
    break;
  case eVAR_TYPE::kLongLong:
    m_size = sizeof(long long);
    break;
  case eVAR_TYPE::kFloat:
    m_size = sizeof(float);
    break;
  case eVAR_TYPE::kDouble:
    m_size = sizeof(double);
    break;
  case eVAR_TYPE::kLongDouble:
    m_size = sizeof(long double);
    break;
  case eVAR_TYPE::kBool:
    m_size = sizeof(bool);
    break;
  case eVAR_TYPE::kChar:
    m_size = sizeof(char);
    break;
  case eVAR_TYPE::kString:
    m_size = sizeof(char*);
    break;
  case eVAR_TYPE::kWChar:
    m_size = sizeof(wchar_t);
    break;
  case eVAR_TYPE::kWString:
    m_size = sizeof(wchar_t*);
    break;
  case eVAR_TYPE::kMixerPointer:
    m_size = sizeof(void*);
    break;
  case eVAR_TYPE::kMixerChannelPointer:
    m_size = sizeof(void*);
    break;
  }
  m_data.resize(m_size, 0);
}

void
VariableClass::setData(void* data)
{
  memcpy(m_data.data(), data, m_size);
}
