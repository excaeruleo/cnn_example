#ifndef TYPEDEF_HPP
#define TYPEDEF_HPP

// TODO: remove
#define NVP(a) BOOST_SERIALIZATION_NVP(a)

#include <vector>

namespace cnn {

typedef unsigned int int_type;
typedef float real_type;

// univariate real_type function
using unary_real_function = real_type(*)(real_type);
// vector + vector to scalar real_type function
using inner_product_real_function =
  real_type(*)(const std::vector<real_type>&, const std::vector<real_type>&);

}  // namespace cnn

#endif  // TYPEDEF_HPP
