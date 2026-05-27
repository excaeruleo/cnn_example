#ifndef NEURON_HPP
#define NEURON_HPP

#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include "typedef.hpp"

namespace cnn {

// TODO: document + add arithmetic operators
class Neuron {
public:
  real_type value{};

  // ostream operator
  friend std::ostream& operator<<(std::ostream& out, const Neuron& n)
  {
    // TODO: maybe consider not writing a newline
		return out << " : value = " << n.value << "\n";
  }

  /**
   * Return a `Neuron` with a negated value.
   */
  auto operator-() const noexcept
  {
    return Neuron{-value};
  }

private:
  // enable serialize() access
  friend boost::serialization::access;

  template <typename Ar>
	void serialize(Ar& ar, unsigned /*version*/)
  {
		ar & BOOST_SERIALIZATION_NVP(value);
	}
};

}  // namespace cnn

#endif  // NEURON_HPP
