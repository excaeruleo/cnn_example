#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace cnn{

bool check_if_file_exists(const std::string & filename);

template<typename T>
T fprime (const T z){
  return 1.;
}

};

#endif
