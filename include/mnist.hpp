#ifndef MNIST_HPP
#define MNIST_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include "typedef.hpp"

namespace cnn{
// Subroutine to read MNIST file and store values in vectors
void readMNISTFile(const std::string& filename, 
                   std::vector<std::vector<cnn::real_type>>& pixelValues, 
                   std::vector<int>& labels) ;
void readMNISTBinaryFiles(const std::string& imagesFilename, 
                          const std::string& labelsFilename, 
                          std::vector<std::vector<cnn::real_type>>& pixelValues, 
                          std::vector<int>& labels);    
void readMNISTBinaryFilesSubset(const std::string& imagesFilename, 
                               const std::string& labelsFilename, 
                               std::vector<std::vector<cnn::real_type>>& pixelValues, 
                               std::vector<int>& labels,
                               int subsetSize,
                               bool randomAccess = false);    
}


#endif
