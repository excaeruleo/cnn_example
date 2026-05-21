#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <optional>
#include <vector>

#include "exccnn/dllexport.h"
#include "typedef.hpp"

namespace cnn {

// Add these declarations before the activation functions
EXCCNN_PUBLIC
real_type kaiming_uniform_init(int fan_in);

EXCCNN_PUBLIC
real_type xavier_uniform_init(int fan_in, int fan_out);

EXCCNN_PUBLIC
std::vector<std::vector<real_type>> orthogonal_init(int rows, int cols);

// Activation functions
EXCCNN_PUBLIC
real_type linear(const real_type);

EXCCNN_PUBLIC
real_type derivative_linear(const real_type);

EXCCNN_PUBLIC
real_type sigmoid(const real_type);

EXCCNN_PUBLIC
real_type derivative_sigmoid(const real_type);

EXCCNN_PUBLIC
real_type relu(const real_type);

EXCCNN_PUBLIC
real_type derivative_relu(const real_type);

// Add softmax declarations
EXCCNN_PUBLIC
real_type softmax(const real_type x);

EXCCNN_PUBLIC
real_type derivative_softmax(const real_type x);

// Optional: Vector version of softmax (often useful)
EXCCNN_PUBLIC
std::vector<real_type> softmax_vector(const std::vector<real_type>& x);

// Cost functions
EXCCNN_PUBLIC
real_type cross_entropy_loss(const std::vector<real_type>& predicted,
                             const std::vector<real_type>& target);

EXCCNN_PUBLIC
real_type mse_loss(const std::vector<real_type>& predicted,
                   const std::vector<real_type>& target);

EXCCNN_PUBLIC
real_type mae_loss(const std::vector<real_type>& predicted,
                   const std::vector<real_type>& target);

EXCCNN_PUBLIC
real_type huber_loss(const std::vector<real_type>& predicted,
                     const std::vector<real_type>& target,
                     real_type delta = 1.0);

EXCCNN_PUBLIC
real_type binary_cross_entropy_loss(const std::vector<real_type>& predicted,
                                    const std::vector<real_type>& target);

EXCCNN_PUBLIC
real_type hinge_loss(const std::vector<real_type>& predicted,
                     const std::vector<real_type>& target);

EXCCNN_PUBLIC
real_type kl_divergence_loss(const std::vector<real_type>& predicted,
                             const std::vector<real_type>& target);

typedef real_type (*CostFunctionPointer)(const std::vector<real_type>&,
                                         const std::vector<real_type>&);

}  // namespace cnn

#endif  // FUNCTIONS_HPP
