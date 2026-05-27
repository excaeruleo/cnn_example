#include <cstdlib>
#include <fstream>
#include <iostream>

#include <boost/serialization/nvp.hpp>
#include "yml_oarchive.hpp"

#include "typedef.hpp"
#include "connector.hpp"
#include "functions.hpp"
#include "layer.hpp"
#include "nn.hpp"
#include "sparse_array.hpp"

class simple_neural_network : public cnn::NeuralNetwork {
public:
  bool initialize() override;
};

bool simple_neural_network::initialize()
{
  n_layers = 2;

  cnn::Layer input_layer = cnn::Layer(5);
  input_layer[0] = 0.;
  input_layer[1] = 0.;
  input_layer[2] = 0.;
  input_layer[3] = 0.;
  input_layer[4] = 1.;
  cnn::Layer output_layer = cnn::Layer(5);
  output_layer[0] = 0.;
  output_layer[1] = 0.;
  output_layer[2] = 0.;
  output_layer[3] = 0.;
  output_layer[4] = 0.;
  layers = {input_layer, output_layer};
  expected = cnn::Layer(5);
  expected[0] = 0.;
  expected[1] = 0.;
  expected[2] = 0.;
  expected[3] = 0.;
  expected[4] = 1.;

  using it = weight_index_type;

  for (cnn::int_type j = 0; j < 5; j ++ ) {
    for (cnn::int_type k = 0; k < 5; k ++ ) {
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
    std::ofstream ofs{"trained3.yml"};
    boost::archive::yml_oarchive yoa{ofs};
    yoa << BOOST_SERIALIZATION_NVP(nn);
  }
  return EXIT_SUCCESS;
}
