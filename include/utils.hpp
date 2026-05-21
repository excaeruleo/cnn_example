#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include "exccnn/dllexport.h"

namespace cnn {

EXCCNN_PUBLIC
bool check_if_file_exists(const std::string& filename);

template<typename T>
T fprime (const T z){
  return 1.;
}

}  // namespace cnn

#endif  // UTILS_HPP
