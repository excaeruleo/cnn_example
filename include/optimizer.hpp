#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

#include <ostream>
#include <string>
#include <tuple>
#include <utility>

#include "typedef.hpp"
#include "functions.hpp"

namespace cnn {

/* Provide function pointers to activation function and derivative of activation function */
class optimizer {
public:
  std::string name;
  // TODO: replace with activation as single member
  unary_real_function activate;
  unary_real_function derivative_funcptr;

  // TODO: split into save/load where load uses to_activation()
  template<class Ar>
  void serialize(Ar& ar, unsigned /*version*/)
  {
    ar & BOOST_SERIALIZATION_NVP(name);
  }

  /**
   * Default ctor.
   *
   * This initializes the `"linear"` activation function.
   */
  optimizer()
    : name{"linear"}, activate{linear}, derivative_funcptr{derivative_linear}
  {}

  // construct from name
  /**
   * Ctor.
   *
   * Construct from a valid activation function identifier.
   *
   * @param name Activation function, e.g. `"relu"`, `"linear"`, etc.
   */
  optimizer(std::string name) : name{std::move(name)}
  {
    // FIXME: name shadowing is tricky
    std::tie(activate, derivative_funcptr) = get_activation(this->name).pair();
  }

  friend std::ostream& operator<<(std::ostream& out, const optimizer& o)
  {
    return out << "optimizer = " << o.name;
  }
};

}  // namespace cnn

#endif  // OPTIMIZER_HPP
