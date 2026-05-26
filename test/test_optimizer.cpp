/**
 * @file test_optimizer.cpp
 * @author David Huang, Derek HUang
 * @brief C++ program testing optimizer construction + stream
 * @copyright MIT License
 */

#include <cstdlib>
#include <iostream>

#include "optimizer.hpp"

int main()
{
  // create a few optimizers with different activations
  cnn::optimizer opt_1{"linear"};
  cnn::optimizer opt_2{"relu"};
  cnn::optimizer opt_3{"sigmoid"};
  // stream
  std::cout <<
    opt_1 << "\n" <<
    opt_2 << "\n" <<
    opt_3 << "\n" << std::flush;
  return EXIT_SUCCESS;
}
