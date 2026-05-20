#include <cstdlib>
#include <fstream>
#include <iostream>
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
  // number of found files
  int n_found = 0;
  // iterate through argv, printing and tracking whether found or not
  for (int i = 1; i < argc; i++) {
    auto found = check_If_File_Exists(argv[i]);
    std::cout << "found " << argv[i] << ": " << found << std::endl;
    n_found += found;
  }
  // success if n_found == argc - 1
  return (n_found == argc - 1) ? EXIT_SUCCESS : EXIT_FAILURE;
}
