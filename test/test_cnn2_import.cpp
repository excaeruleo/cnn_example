// Test program to read a yaml file and generate a neural network accordingly

#include "yml_oarchive.hpp"
#include "yml_iarchive.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp> // serialize boost::optional

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "typedef.hpp"
#include "connection.hpp"
#include "neuron.hpp"
#include "layer.hpp"
#include "utils.hpp"
#include "weights.hpp"
#include "nn.hpp"

#define NVP(a) BOOST_SERIALIZATION_NVP(a) 

class simple_neural_network : public cnn::NeuralNetwork{

public:
  bool initialize();

};

bool simple_neural_network::initialize(){

	//n_layers = 2;

	//cnn::Layer input_layer = cnn::Layer(4); // make a input layer of 4 neurons.
  //input_layer[0] = 1.;
  //input_layer[1] = 0.;
  //input_layer[2] = 0.;
  //input_layer[3] = 0.;
	//cnn::Layer output_layer = cnn::Layer(4); // make a output layer of 4 neurons.
  //output_layer[0] = 0.;
  //output_layer[1] = 0.;
  //output_layer[2] = 0.;
  //output_layer[3] = 0.;
	//layers = {input_layer, output_layer};
	//expected = cnn::Layer(4); // make a output layer of 4 neurons.
  //expected[0] = 1.;
  //expected[1] = 0.;
  //expected[2] = 0.;
  //expected[3] = 0.;

  //std::cout << "input: " << layers[0];
  //std::cout << "output: " << layers[1];
  //std::cout << "weights: " << weights;

	return true;

}

int main(){

	simple_neural_network nn;
	{
		std::ifstream ifs{"trained.yml"};
		boost::archive::yml_iarchive yia{ifs};
		yia
			>> NVP(nn)
		;
	}
  nn.update();
  std::cout << "weights: " << nn.weights;

	{
		std::ofstream ofs{"trained.yml"};
		boost::archive::yml_oarchive yoa{ofs};
		yoa
			<< NVP(nn)
		;
	}

}
