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
#include "mnist.hpp"
#include "functions.hpp"
#include "optimizer.hpp"

#define NVP(a) BOOST_SERIALIZATION_NVP(a) 

class simple_neural_network : public cnn::NeuralNetwork{

public:
  bool initialize();
  bool reset_input(const std::vector<std::vector<cnn::real_type>> & input, const int index);
  bool reset_expected(const int & index);

};

bool simple_neural_network::initialize(){

  n_layers = 3;
  int n_input = 784; /* for the 784 pixels in 18x18 picture */
  int n_output = 10; /* for the 10 digits, 0,1,...9 */
  int n_hidden = 100; /* hidden layer size */

  cnn::Layer input_layer = cnn::Layer(n_input); 
  input_layer.layer_name="input layer";

  cnn::Layer hidden_layer = cnn::Layer(n_hidden); 
  hidden_layer.layer_name="hidden layer";

  cnn::Layer output_layer = cnn::Layer(n_output);
  output_layer.layer_name="output layer";

  layers = {input_layer, hidden_layer, output_layer};
  expected = cnn::Layer(n_output); 

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

/* reset the input to the image file pixel value */
bool 
simple_neural_network::reset_input(const std::vector<std::vector<cnn::real_type>> & input, const int index){
	std::cout << layers[0].size() << " " << input[index].size();
	assert(this->layers[0].size() == input[index].size());
	for(int i = 0; i < this->layers[0].size(); i ++)
		this->layers[0][i]=input[index][i];
	return true;
}
/* reset the out to the expected digit */
bool
simple_neural_network::reset_expected(const int & index){
	assert(this->layers[2].size() > index);
	for(int i = 0; i < this->layers[2].size(); i ++)
		this->layers[2][i]=0;
  this->layers[2][index]=1;
	return true;
}

int main(){

	std::string filename = "testinput/mnist/mnist_train_1000.txt"; // Replace with your file name
	std::vector<std::vector<cnn::real_type>> pixelValues;
	std::vector<int> labels;

	try {
			// Read the MNIST file
			cnn::readMNISTFile(filename, pixelValues, labels);

			// Output confirmation
			std::cout << "Successfully read " << pixelValues.size() << " images with "
								<< pixelValues[0].size() << " pixels each and " << labels.size() 
								<< " labels." << std::endl;

			// Optional: Print first image's pixels and label for verification
			std::cout << "First image pixels: ";
			for (int i = 0; i < 784; ++i) {
					std::cout << pixelValues[0][i] << " ";
			}
			std::cout << "\nLabel: " << labels[0] << std::endl;

	} catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return 1;
	}


  simple_neural_network nn;

	// Train the neural network with the 1000 images as training set
	nn.initialize();
	nn.set_optimizer(cnn::optimizer("sigmoid", cnn::sigmoid, cnn::derivative_sigmoid));
  for (int i = 0; i < 1000; i ++){
		std::cout << "processing mnist training set #" << i << std::endl;
    nn.reset_input(pixelValues, i);
    nn.reset_expected(labels[i]);
		nn.update();
		std::cout << nn.cost() << std::endl;
	}

  {
    std::ofstream ofs{"trained.yml"};
    boost::archive::yml_oarchive yoa{ofs};
    yoa
      << NVP(nn)
    ;
  }

}
