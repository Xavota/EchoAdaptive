#pragma once
#include <vector>

using Byte = char;

namespace eVAR_TYPE {
enum E
{
  kNone = 0,
  kInt,
  kShortInt,
  kLongInt,
  kLongLong,
  kFloat,
  kDouble,
  kLongDouble,
  kBool,
  kChar,
  kString,
  kWChar,
  kWString,
  kMixerPointer,
  kMixerChannelPointer
};
}

class VariableClass
{
 public:
  VariableClass() = default;
  ~VariableClass() = default;

  void
  setType(eVAR_TYPE::E type);
  void
  setData(void* data);

 private:
  eVAR_TYPE::E m_type;
  int m_size;
  std::vector<Byte> m_data;
};

class VariablesSystem
{
 public:
  VariablesSystem() = default;
  ~VariablesSystem() = default;

 private:
  
};

