#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <optional>
#include <string_view>
#include <utility>
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

/**
 * Structure holding an activation function and its derivative.
 */
struct activation {
  unary_real_function f;  // function
  unary_real_function g;  // gradient

  // convert into std::pair for std::tie interop
  std::pair<unary_real_function, unary_real_function> pair() const noexcept
  {
    return {f, g};
  }
};

/**
 * Obtain the activation + derivative function given a string identifier.
 *
 * @param name Activation name, e.g. `"linear"`, `"relu"`, etc.
 */
EXCCNN_PUBLIC
activation get_activation(std::string_view name);

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

// TODO: deprecate in favor of inner_product_real_function
typedef real_type (*CostFunctionPointer)(const std::vector<real_type>&,
                                         const std::vector<real_type>&);

/**
 * Obtain the cost function given a string identifier.
 *
 * @param name Cost function name, i.e. "hinge_loss", "mse_loss", etc.
 */
EXCCNN_PUBLIC
inner_product_real_function get_cost_function(std::string_view name);

/**
 * Obtain the cost function identifier given the cost function.
 *
 * @param func Cost function
 */
EXCCNN_PUBLIC
std::string_view get_cost_function_name(inner_product_real_function func);

}  // namespace cnn

#endif  // FUNCTIONS_HPP
