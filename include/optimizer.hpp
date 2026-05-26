#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
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

  /**
   * Boost serialization function.
   *
   * This saves just the name of the `optimizer` object.
   *
   * @tparam Ar Boost.Serialization output archive
   *
   * @param ar Output archive
   */
  template <typename Ar>
  void save(Ar& ar, unsigned /*version*/) const
  {
    ar & BOOST_SERIALIZATION_NVP(name);
  }

  /**
   * Boost de-serialization function.
   *
   * This loads the `optimizer` name and then uses it set the activation.
   *
   * @tparam Ar Boost.Serialization input archive
   *
   * @param ar Input archive
   */
  template <typename Ar>
  void load(Ar& ar, unsigned /*version*/)
  {
    ar & BOOST_SERIALIZATION_NVP(name);
    std::tie(activate, derivative_funcptr) = get_activation(name).pair();
  }

  // implement serialize()
  BOOST_SERIALIZATION_SPLIT_MEMBER()

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
