#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

#include "typedef.hpp"
#include "connector.hpp"

namespace cnn {

class Connection {
public:
  // TODO: can be private if Connector has operator[] and iterator support
  std::vector<cnn::Connector> weights;

private:
  friend class boost::serialization::access;

	template <typename Ar>
	void serialize(Ar& ar, unsigned /*version*/)
  {
		ar & BOOST_SERIALIZATION_NVP(weights);
	}

public:
  // TODO: compare Connectors as well
  bool operator==(Connection const & con_) const
  {
    return con_.weights.size() == weights.size();
  }

  Connection() = default;

  Connection(const std::vector<cnn::Connector> & _weights){
     weights = _weights;
  }

  // ostream operator
  friend std::ostream& operator<< (std::ostream& stream, const Connection & con) {

    for (int i = 0; i < con.weights.size(); i ++)
      stream << con.weights[i];
    return stream;
  }
  void update_forward(const real_type & /*input*/) {

  }
  void update_backward(
    const real_type & /*previous*/,
    const real_type & /*expected*/,
    const real_type & /*next*/) {

  }
};

}  // namespace cnn

#endif  // CONNECTION_HPP
