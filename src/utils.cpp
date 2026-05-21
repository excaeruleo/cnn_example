#include "utils.hpp"

#include <fstream>

namespace cnn {

bool check_if_file_exists(const std::string & filename) {
  std::ifstream file;
  file.open(filename);
  if (file) {
    return true;
  }
  return false;
}

}  // namespace cnn
