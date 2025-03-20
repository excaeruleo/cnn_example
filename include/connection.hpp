#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "yml_oarchive.hpp"
#include "yml_iarchive.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp> // serialize boost::optional

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <vector>

#include <stdio.h> 
#include <stdlib.h> 
#include <cassert>

#include "typedef.hpp"
#include "connector.hpp"

#define NVP(a) BOOST_SERIALIZATION_NVP(a)

using namespace boost::numeric::ublas;

namespace cnn{

class Connection{

public:
  std::vector<cnn::Connector> weights;

  friend std::ostream & operator << (std::ostream & stream, const Connection & con_);
	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar & NVP(weights);
	}
  bool operator == (Connection const & con_) const { return con_.weights.size() == weights.size() ; }

  Connection(){
     
  }
  
  Connection(const std::vector<cnn::Connector> & _weights){ 
     weights = _weights;
  }

  // ostream operator
  friend std::ostream& operator<< (std::ostream& stream, const Connection & con) {

    for (int i = 0; i < con.weights.size(); i ++)
      stream << con.weights[i];
    return stream;
  }
  void update_forward(const real_type & input) {

  }
  void update_backward(const real_type & previous, const real_type & expected, const real_type & next){

  }
  
private:
};

};

#endif

