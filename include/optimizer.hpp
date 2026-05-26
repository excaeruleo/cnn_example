#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <boost/serialization/access.hpp>
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

/**
 * Layer optimizer class.
 *
 * This holds function pointers to the activation function and its derivative.
 */
class optimizer {
public:
  /**
   * Default ctor.
   *
   * This initializes the `"linear"` activation function.
   */
  optimizer() : name_{"linear"}, act_{linear, derivative_linear} {}

  /**
   * Ctor.
   *
   * Construct from a valid activation function identifier.
   *
   * @param name Activation function, e.g. `"relu"`, `"linear"`, etc.
   */
  optimizer(std::string name)
    : name_{std::move(name)}, act_{get_activation(name_)}
  {}

  /**
   * Return the name of the layer activation.
   */
  auto& name() const noexcept { return name_; }

  /**
   * Invoke the activation function.
   */
  real_type activate(real_type x) const
  {
    return act_.f(x);
  }

  /**
   * Invoke the activation function derivative.
   */
  real_type derivative(real_type x) const
  {
    return act_.g(x);
  }

  friend std::ostream& operator<<(std::ostream& out, const optimizer& opt)
  {
    return out << "optimizer = " << opt.name();
  }

private:
  std::string name_;  // activation name
  activation act_;    // activation function + derivative pointers

  // enable save/load access
  friend class boost::serialization::access;

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
    ar & boost::serialization::make_nvp("name", name_);
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
    ar & boost::serialization::make_nvp("name", name_);
    act_ = get_activation(name_);
  }

  // implement serialize()
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

}  // namespace cnn

#endif  // OPTIMIZER_HPP
