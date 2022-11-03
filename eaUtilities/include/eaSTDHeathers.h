#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

namespace eaSdkEngine {
using String = std::string;
using FStream = std::fstream;
using IFStream = std::ifstream;
using OFStream = std::ofstream;

template<typename T>
using Vector = std::vector<T>;

template<typename K, typename V>
using Map = std::map<K, V>;
}