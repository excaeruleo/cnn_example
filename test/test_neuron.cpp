#include <cstdlib>
#include <iostream>

#include "neuron.hpp"

int main()
{
  cnn::Neuron n{1.5f};
  // note: operator<< for Neuron includes trailing newline
  std::cout << n << std::flush;
  // update + stream again
  n = -n;
  std::cout << n << std::flush;
  return EXIT_SUCCESS;
}
