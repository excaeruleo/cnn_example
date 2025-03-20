#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <vector>

#include "typedef.hpp"
#include "functions.hpp"

namespace cnn{
// Add these declarations before the activation functions
cnn::real_type kaiming_uniform_init(int fan_in);
cnn::real_type xavier_uniform_init(int fan_in, int fan_out);
std::vector<std::vector<real_type>> orthogonal_init(int rows, int cols);

// Activation functions
cnn::real_type linear(const cnn::real_type);
cnn::real_type derivative_linear(const cnn::real_type);
cnn::real_type sigmoid(const cnn::real_type);
cnn::real_type derivative_sigmoid(const cnn::real_type);
cnn::real_type relu(const cnn::real_type);
cnn::real_type derivative_relu(const cnn::real_type);
// Add softmax declarations
cnn::real_type softmax(const cnn::real_type x);
cnn::real_type derivative_softmax(const cnn::real_type x);

// Optional: Vector version of softmax (often useful)
std::vector<cnn::real_type> softmax_vector(const std::vector<cnn::real_type>& x);

// Cost functions
cnn::real_type cross_entropy_loss(const std::vector<cnn::real_type>& predicted, 
                                   const std::vector<cnn::real_type>& target);
cnn::real_type mse_loss(const std::vector<cnn::real_type>& predicted, 
                           const std::vector<cnn::real_type>& target);
cnn::real_type mae_loss(const std::vector<real_type>& predicted, 
                           const std::vector<real_type>& target);
cnn::real_type huber_loss(const std::vector<real_type>& predicted, 
                             const std::vector<real_type>& target,
                             real_type delta = 1.0);
cnn::real_type binary_cross_entropy_loss(const std::vector<real_type>& predicted, 
                                           const std::vector<real_type>& target);
cnn::real_type hinge_loss(const std::vector<real_type>& predicted, 
                             const std::vector<real_type>& target);
cnn::real_type kl_divergence_loss(const std::vector<real_type>& predicted, 
                                     const std::vector<real_type>& target);


typedef cnn::real_type (*CostFunctionPointer)(const std::vector<cnn::real_type>&, 
                                               const std::vector<cnn::real_type>&);
 
}

#endif
