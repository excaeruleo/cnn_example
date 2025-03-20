#include <iostream>
#include <fstream>
#include <string>
#include <vector>

bool check_If_File_Exists(const std::string & filename) {
  std::ifstream file;
  file.open(filename);
  if (file) {
    return true;
  }
  return false;
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    std::cout << check_If_File_Exists(argv[i]) << std::endl;
  }
  /*
  std::ifstream file;
  std::string workingPath = argv[1];
  std::string bogusPath = argv[2];
  std::string paths[2] = {workingPath, bogusPath};
  for (auto i : paths) {
    file.open(i);
    if (file) {
      std::cout << i << " exists." << std::endl;
    }
    else {
      std::cout << i << " does not exist." << std::endl;
    }
  }
  */
  return 0;
}
