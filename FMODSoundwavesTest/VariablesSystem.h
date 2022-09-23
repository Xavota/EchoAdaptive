#pragma once
#include <vector>

using Byte = char;

class VariableClass
{
 public:
  VariableClass() = default;
  ~VariableClass() = default;

  template<typename T>
  void
  setData(const T& data);
  template<typename T>
  const T&
  getData();

 private:
  std::vector<Byte> m_data;
};
template<typename T>
void
VariableClass::setData(const T& data)
{
  m_data.clear();
  m_data.resize(sizeof(T), 0);
  memcpy(m_data.data(), &data, sizeof(T));
}
template<typename T>
const T&
VariableClass::getData()
{
  return *reinterpret_cast<T*>(m_data.data());
}

