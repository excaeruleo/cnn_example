#ifndef NEURON_HPP
#define NEURON_HPP

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

#define NVP(a) BOOST_SERIALIZATION_NVP(a)

using namespace boost::numeric::ublas;

namespace cnn{


class Neuron{

public:
  real_type value;

  real_type get() const {
    return value;
  }
  void set_value(const real_type value_) {
    value = value_;
  }
	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar &  NVP(value);
	}
  Neuron(){ 
     default_initialize();
  }

  // ostream operator
  friend std::ostream& operator<< (std::ostream& stream, const Neuron & n) {

		std::cout  << " : value = " << n.value << std::endl;

    return stream;
  }
  
private:
  void default_initialize(){
    value = 0.;
  }
};

};

#endif

