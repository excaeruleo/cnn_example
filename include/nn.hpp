#ifndef NN_HPP
#define NN_HPP

#include "yml_oarchive.hpp"
#include "yml_iarchive.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp> // serialize boost::optional

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <algorithm>

#include <stdio.h> 
#include <stdlib.h> 
#include <cassert>

#include "typedef.hpp"
#include "connector.hpp"
#include "sparse_array.hpp"

#define NVP(a) BOOST_SERIALIZATION_NVP(a)

using namespace boost::numeric::ublas;

namespace cnn{

// A basic neural network base class, it must be extended with customized
// initialize method
class NeuralNetwork{

public:
  int n_layers;
	cnn::Layer expected;
	std::vector< cnn::Layer > layers;
  cnn::Weights weights;
  int_type epoch_frequency;
  cnn::CostFunctionPointer cost_function;

	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar & NVP(n_layers) & NVP(expected) & NVP(layers) & NVP(weights) & NVP(epoch_frequency);
	}
	bool operator==(NeuralNetwork const& nn_) const{
    return nn_.n_layers==n_layers
        && nn_.layers  == layers;
  }

  virtual ~NeuralNetwork() = default;

  // ostream operator
  friend std::ostream& operator<< (std::ostream& stream, const NeuralNetwork & nn) {

		std::cout << "n_layers = " << nn.n_layers << std::endl;

		for(int i = 0; i < nn.layers.size(); i ++)
			std::cout << nn.layers[i] << '\n';

    return stream;
  }

  void print_weight_statistics() const {
    weights.print_weight_statistics();
  }

  virtual bool initialize() = 0; // Pure virtual function so this class must be extended.

	void set_optimizer(const optimizer & o){
#ifdef DEBUG
		std::cout << "nn::set_optimizer = " << o << std::endl;
#endif
		for(int_type i = 0; i < layers.size(); i ++){
#ifdef DEBUG
			std::cout << "layers[" << i << "].set_optimizer = " << o << std::endl;
#endif
			layers[i].set_optimizer(o);
		}
	}

  //cnn::real_type sigmoid(const cnn::real_type x) const{
  //  return 1/(1 + exp(-x));
  //}

  cnn::real_type cost(void) const {
    cnn::real_type sum = 0.;
    cnn::int_type output_layer = layers.size() - 1;
    for (cnn::int_type j = 0; j < expected.size(); j ++ )
      sum = sum + 0.5 * pow(layers[output_layer][j] - expected[j], 2);
    return sum;
  }

  void update_forward(void){
    
#ifdef DEBUG
		std::cout << "input: " << layers[0];
#endif
    //iteratively update the neural network forward from 2nd layer to last layer
    //l = 0 is first layer, l = 1 is 2nd layer
    int_type n_layers = layers.size();
    for (int_type l = 1; l < n_layers ; l++){
      layers[l].update_forward(l, layers[l-1], weights);
#ifdef DEBUG
      std::cout << "output: l=" << l << " : " << layers[l];
#endif
    }
  }

  void update_backward(void) {
#ifdef DEBUG
    std::cout << "prior to update_backward weights: \n" << weights;
#endif
    // Calculate delta from output layer and expected value
    std::vector<cnn::real_type> delta;
    std::vector<cnn::real_type> prev_delta;
    int_type n_layers = layers.size();
    int_type layer_neuron_size = layers[layers.size()-1].size();
    delta.resize(layer_neuron_size);
    for (int j = 0; j < layer_neuron_size; j ++){
      //delta[j] = 2*(layers[1][j]-expected[j])*sigmoid(layers[1][j])*(1 - sigmoid(layers[1][j]));
      delta[j] = (layers[n_layers-1][j]-expected[j]);
#ifdef DEBUG
      printf("l=%d, j=%d: %f %f delta[j]=%f\n", n_layers-1, j, layers[n_layers-1][j], expected[j], delta[j]);
#endif
    }

    //iteratively update the neural network backward from last layer to 2nd layer
    //l = n_layers - 1 is the last layer (output layer), l = 1 is the 2nd layer (the layer after input layer)
    for (int_type l = n_layers - 1; l > 0; l--){
      layers[l].update_backward(l, layers[l-1], weights, delta, prev_delta);
      std::swap(delta, prev_delta); // efficiently swap the underlying memory pointer without actually copying
    }
#ifdef DEBUG
    std::cout << "after update_backward weights: \n" << weights;
#endif
  }

  void update(const cnn::int_type max_iter=1000, const cnn::real_type threshold=1.e-6) {
    cnn::int_type iter = 0;
#ifdef DEBUG
    std::cout << "expected = " << expected << std::endl;
    std::cout << "layers.back().values() = " << layers.back() << std::endl;
#endif
    cnn::real_type mse = cost_function(layers.back().values(), expected.values());
    if(epoch_frequency < 100)
      epoch_frequency = 100;
    while(mse > threshold and iter < max_iter){
      update_forward();
      update_backward();
      iter ++;
      if(iter % epoch_frequency == 0 or iter < 10){
        std::cout << "epoch: " << iter << " cost = " << mse << std::endl;
      }
      mse = cost();
    }
    if(iter >= 10)
      std::cout << "epoch: " << iter << " cost = " << mse << std::endl;
  }

  std::vector<real_type> get_output() const {
    if (layers.empty()) {
        return {};
    }
    return layers.back().values();
  }

  void reset_learning_parameters(const real_type learning_rate = 0.1, 
                                 const real_type momentum = 0.01, 
                                 const real_type decay = 0.001) {
        // Reset all weights' learning parameters
        for (auto& weight_pair : weights.w) {
            weight_pair.second.reset(learning_rate, momentum, decay);
        }
#ifdef DEBUG
        std::cout << "Reset learning parameters:"
                  << "\n  Learning rate: " << learning_rate
                  << "\n  Momentum: " << momentum
                  << "\n  Decay: " << decay << std::endl;
#endif
    }

    // Method to adjust learning rate during training
    void adjust_learning_rate(real_type factor) {
        for (auto& weight_pair : weights.w) {
            auto& connector = weight_pair.second;
            connector.learning_rate *= factor;
        }
#ifdef DEBUG
        std::cout << "Adjusted learning rate by factor: " << factor << std::endl;
#endif
    }

    // Get current learning rate (average across all connectors)
    real_type get_current_learning_rate() const {
        if (weights.w.empty()) {
            return 0.0;
        }
        
        real_type sum_lr = 0.0;
        for (const auto& weight_pair : weights.w) {
            sum_lr += weight_pair.second.learning_rate;
        }
        return sum_lr / weights.w.size();
    }

    // Reset the entire network state
    void reset_network(const real_type learning_rate = 0.1,
                      const real_type momentum = 0.01,
                      const real_type decay = 0.001) {
        // Reset learning parameters
        reset_learning_parameters(learning_rate, momentum, decay);
        
        // Clear layer values
        for (auto& layer : layers) {
            std::fill(layer.values().begin(), layer.values().end(), 0.0);
        }
        
        // Reset expected values
        std::fill(expected.values().begin(), expected.values().end(), 0.0);
        
#ifdef DEBUG
        std::cout << "Network reset completed" << std::endl;
#endif
    }

    // Optional: Add a method to reset weights to their initial values
    void reinitialize_weights() {
        // Store the current network architecture
        std::vector<int> layer_sizes;
        for (const auto& layer : layers) {
            layer_sizes.push_back(layer.size());
        }
        
        // Reinitialize weights using the stored architecture
        weights.w.clear();
        for (size_t l = 1; l < layer_sizes.size(); ++l) {
            for (int j = 0; j < layer_sizes[l]; ++j) {
                for (int k = 0; k < layer_sizes[l-1]; ++k) {
                    weights.w[weight_index_type{static_cast<unsigned int>(l), j, k}] = 
                        Connector(l, j, k, layer_sizes[l-1], layer_sizes[l]);
                }
            }
        }
#ifdef DEBUG
        std::cout << "Weights reinitialized" << std::endl;
#endif
    }

};

};

#endif
