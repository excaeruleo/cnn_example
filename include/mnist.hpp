#ifndef MNIST_HPP
#define MNIST_HPP

#include <string>
#include <vector>


#include "exccnn/dllexport.h"
#include "typedef.hpp"

namespace cnn {

// Subroutines to read MNIST file and store values in vectors

EXCCNN_PUBLIC
void readMNISTFile(const std::string& filename,
                   std::vector<std::vector<real_type>>& pixelValues,
                   std::vector<int>& labels);

EXCCNN_PUBLIC
void readMNISTBinaryFiles(const std::string& imagesFilename,
                          const std::string& labelsFilename,
                          std::vector<std::vector<real_type>>& pixelValues,
                          std::vector<int>& labels);

EXCCNN_PUBLIC
void readMNISTBinaryFilesSubset(const std::string& imagesFilename,
                               const std::string& labelsFilename,
                               std::vector<std::vector<real_type>>& pixelValues,
                               std::vector<int>& labels,
                               int subsetSize,
                               bool randomAccess = false);

}  // namespace cnn

#endif  // MNIST_HPP
