// Test program to read a yaml file and generate a neural network accordingly

#include "yml_oarchive.hpp"
#include "yml_iarchive.hpp"

#include <boost/serialization/nvp.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "nn.hpp"

class simple_neural_network : public cnn::NeuralNetwork {
public:
  bool initialize() override;
};

bool simple_neural_network::initialize()
{
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
	return true;
}

int main()
{
  // populate from YAML archive
	simple_neural_network nn;
	{
		std::ifstream ifs{"trained2.yml"};
		boost::archive::yml_iarchive yia{ifs};
		yia >> BOOST_SERIALIZATION_NVP(nn);
	}
  // print input, output, weights
  std::cout <<
    "input: " << nn.layers[0] <<
    "output: " << nn.layers[1] <<
    "weights: " << nn.weights << std::flush;
  // update + print weights again
  nn.update();
  std::cout << "weights: " << nn.weights;
  // write back to YAML archive
	{
		std::ofstream ofs{"trained2.yml"};
		boost::archive::yml_oarchive yoa{ofs};
		yoa << BOOST_SERIALIZATION_NVP(nn);
	}
  return EXIT_SUCCESS;
}
