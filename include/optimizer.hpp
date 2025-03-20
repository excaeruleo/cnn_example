#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include "yml_oarchive.hpp"
#include "yml_iarchive.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp> // serialize boost::optional

#define NVP(a) BOOST_SERIALIZATION_NVP(a)

#include <string>
#include "typedef.hpp"
#include "functions.hpp"

namespace cnn{

	/* Provide function pointers to activatation function and derivative of activation function */
	class optimizer{
		public:
      typedef cnn::real_type (*FunctionPointer)(cnn::real_type);

      std::string name;
	    FunctionPointer activate;
	    FunctionPointer derivative_funcptr;

		template<class Ar>
		void serialize(Ar& ar, unsigned){
			ar & NVP(name);
		}

		// Default constructor using linear function
		optimizer(): name ("linear"), activate(linear), derivative_funcptr(derivative_linear) {}
		// Initialize with a new optimizer
		optimizer(std::string name, cnn::real_type (*funcptr) (cnn::real_type), cnn::real_type (*derivative_funcptr) (cnn::real_type) ): name (name), activate(funcptr), derivative_funcptr(derivative_funcptr){
		}
		optimizer(const optimizer & o): name (o.name), activate(o.activate), derivative_funcptr(o.derivative_funcptr) {}
		friend std::ostream& operator<< (std::ostream& stream, const optimizer & o) {
			std::cout << "optimizer = " << o.name << std::endl;
			return stream;
		}
	};
}

#endif
