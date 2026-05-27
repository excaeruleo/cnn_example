// Test program to read a yaml file and generate a neural network accordingly

#include "yml_oarchive.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "connector.hpp"
#include "functions.hpp"
#include "layer.hpp"
#include "nn.hpp"
#include "sparse_array.hpp"

class simple_neural_network : public cnn::NeuralNetwork {

public:
  bool initialize() override;
};

bool simple_neural_network::initialize() {

	n_layers = 2;

	cnn::Layer input_layer = cnn::Layer(4); // make a input layer of 4 neurons.
  input_layer[0] = 0.;
  input_layer[1] = 0.;
  input_layer[2] = 1.;
  input_layer[3] = 0.;
	cnn::Layer output_layer = cnn::Layer(4); // make a output layer of 4 neurons.
  output_layer[0] = 0.;
  output_layer[1] = 0.;
  output_layer[2] = 0.;
  output_layer[3] = 0.;
	layers = {input_layer, output_layer};
	expected = cnn::Layer(4); // make a output layer of 4 neurons.
  expected[0] = 0.;
  expected[1] = 0.;
  expected[2] = 1.;
  expected[3] = 0.;

  using it = weight_index_type;

	for (cnn::int_type j = 0; j < 4; j ++ ){
		for (cnn::int_type k = 0; k < 4; k ++ ){
			weights.w[it{1,j,k}] = cnn::Connector(1,j,k);
		}
	}

  // MSE loss
  cost_function("mse_loss");

  std::cout << "input: " << layers[0];
  std::cout << "output: " << layers[1];
  std::cout << "weights: " << weights;

	return true;

}

int main()
{
	simple_neural_network nn;
	nn.initialize();
  nn.update();
	{
		std::ofstream ofs{"trained2.yml"};
		boost::archive::yml_oarchive yoa{ofs};
		yoa << BOOST_SERIALIZATION_NVP(nn);
	}
  return EXIT_SUCCESS;
}
