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

  n_layers = 4;
  int n_input = 4;
  int n_output = 4;
  int n_hidden = 10;

  cnn::Layer input_layer = cnn::Layer(n_input); 
  input_layer.layer_name="input layer";
  for(int i = 0; i < n_input; i ++)
    input_layer[i] = 0.;
  input_layer[3] = 1.;

  cnn::Layer hidden_layer = cnn::Layer(n_hidden); 
  hidden_layer.layer_name="hidden layer";
  for(int i = 0; i < n_hidden; i ++)
    hidden_layer[i] = 0.;

  cnn::Layer hidden_layer1 = cnn::Layer(n_hidden); 
  hidden_layer1.layer_name="hidden layer 1";
  for(int i = 0; i < n_hidden; i ++)
    hidden_layer1[i] = 0.;

  cnn::Layer output_layer = cnn::Layer(n_output);
  output_layer.layer_name="output layer";
  for(int i = 0; i < n_output; i ++)
    output_layer[i] = 0.;

  layers = {input_layer, hidden_layer, hidden_layer1, output_layer};
  expected = cnn::Layer(n_output); 
  for(int i = 0; i < n_output; i ++)
    expected[i] = 0.;
  expected[3] = 1.;

  using it = weight_index_type;

  for (cnn::int_type j = 0; j < n_hidden; j ++ ){
    for (cnn::int_type k = 0; k < n_input; k ++ ){
      weights.w[it{1,j,k}] = cnn::Connector(1,j,k);
    }
  }

  for (cnn::int_type j = 0; j < n_output; j ++ ){
    for (cnn::int_type k = 0; k < n_hidden; k ++ ){
      weights.w[it{2,j,k}] = cnn::Connector(2,j,k);
    }
  }

  //std::cout << "input: " << layers[0];
  //std::cout << "output: " << layers[1];
  //std::cout << "weights: " << weights;

  return true;

}

int main(){

  simple_neural_network nn;
  nn.initialize();
  nn.update();
  std::cout << nn.cost() << std::endl;

  {
    std::ofstream ofs{"trained.yml"};
    boost::archive::yml_oarchive yoa{ofs};
    yoa
      << NVP(nn)
    ;
  }

}
