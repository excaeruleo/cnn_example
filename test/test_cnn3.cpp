#include <yml_oarchive.hpp>
#include <yml_iarchive.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp>

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

  std::cout << "input: " << layers[0];
  std::cout << "output: " << layers[1];
  std::cout << "weights: " << weights;
  return true;

}

int main() {
  
  simple_neural_network nn;
  nn.initialize();
  nn.update();

  {
    std::ofstream ofs{"trainedOne.yaml"};
    boost::archive::yml_oarchive yoa{ofs};
    yoa 
      << NVP(nn)
    ;
  }

}
