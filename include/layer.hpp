#ifndef LAYER_HPP
#define LAYER_HPP

#include "yml_oarchive.hpp"
#include "yml_iarchive.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp> // serialize boost::optional

#include <vector>
#include <algorithm>

#include <cassert>

#include "typedef.hpp"
#include "neuron.hpp"
#include "connection.hpp"
#include "weights.hpp"
#include "sparse_array.hpp"
#include "utils.hpp"
#include "optimizer.hpp"
#include "functions.hpp"

#define NVP(a) BOOST_SERIALIZATION_NVP(a)

namespace cnn{

//class Neuron; // Forward declaration

class Layer{
  // stores a list of neurons
  std::vector<Neuron> neurons;
  int                 layer_type; //-1: input, 0: middle, 1: output
  int                 layer_size;
  std::vector<real_type> delta; // save a copy of delta for efficient back propogation in recursive algorithm
  optimizer           layer_optimizer;
  
public:
  std::string         layer_name; // numerical, image, video, audio
	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar & NVP(layer_type) & NVP(layer_name) & NVP(layer_size) & NVP(neurons) & NVP(delta) & NVP(layer_optimizer);
    //neurons.resize(layer_size);
	}
	bool operator==(Layer const& l_) const{
    return l_.layer_type==this->layer_type
        && l_.layer_name == this->layer_name
        && l_.layer_size == this->layer_size;
  }

  // serialization library relies on default copy constructor and default constructor
  // disable copy constructor
  //Layer(const Layer &) = delete;

  // Constructors
  Layer(){
    layer_type = 0;
    layer_name = "numerical";
  }
  Layer(const int_type n){
    neurons.resize(n);
    layer_type = 0;
    layer_name = "numerical";
  }
	void set_optimizer(const optimizer & o){
#ifdef DEBUG
		std::cout << "set_optimizer = " << o << std::endl;
#endif
		layer_optimizer = o;
#ifdef DEBUG
		std::cout << "after set_optimizer = " << layer_optimizer << std::endl;
#endif
	}

  // ostream operator
  friend std::ostream& operator<< (std::ostream& stream, const Layer & layer) {

		std::cout << layer.layer_name << " : layer_type = " << layer.layer_type << std::endl;
		std::cout << "layer_optimizer = " << layer.layer_optimizer << std::endl;
		for(int i = 0; i < layer.neurons.size(); i ++)
			std::cout << "layer[" << i <<"]" << layer.neurons[i];

    return stream;
  }

  // return the size of the layer: how many neurons
  cnn::int_type size(void) const {
    return neurons.size();
  }

  // Get
  real_type   operator [] (cnn::int_type i) const { assert (i >= 0 && i < neurons.size()); return neurons[i].value; }
  // Set
  real_type & operator [] (cnn::int_type i) { assert (i >= 0 && i < neurons.size()); return neurons[i].value; }

  // Calculate the output for j-th neuron in the next layer.
  int update_forward(const int_type l, const Layer & prev, Weights & weights){
    // o_j = \sum w_jk i_k
    // input_index: k
    // output_index: j
    // weight: w_jk
    using it = weight_index_type;
    for (cnn::int_type j = 0; j < size(); j ++) {
			real_type sum = 0.;
      for (cnn::int_type k = 0; k < prev.size(); k ++) {
        sum = sum + weights.w[it{l, j, k}].weight*prev[k];
        //sum = sum + weights.w[it{l, j, k}].weight*prev[k] + weights.w[it{l, j, k}].bias;
#ifdef DEBUG
        printf("layer::update_forward:: l=%d,j=%d,k=%d: prev[k]=%f sum=%f \n", l, j, k, prev[k], sum);
#endif
      }
      neurons[j].value = layer_optimizer.activate(sum);
      //printf("%d: %f \n", j, neurons[j].value);
    }

    return 0;
  }

  //void update_backward(int_type l, cnn::Layer layer, std::vector<real_type> delta){
  int update_backward(const int_type l, const Layer & prev, Weights & weights, const std::vector<real_type> & delta, std::vector<real_type> & prev_delta){
    using it = weight_index_type;

    // Calculate delta for previous layer
    // maybe update delta and return it to neural network for recursion?
    this->delta = delta;
    prev_delta.resize(prev.size());
    std::fill(prev_delta.begin(), prev_delta.end(), 0.);
    for(int_type k = 1; k < prev.size(); k ++ ) {
      for(int_type j = 1; j < size(); j ++){
        prev_delta[k] +=  weights.w[it{l, j, k}].weight * delta[j];
      }
      prev_delta[k] *= layer_optimizer.derivative_funcptr(prev[k]);
    }

    // Update weights in current layer
    for (cnn::int_type j = 0; j < size(); j ++) {
      real_type gradient = 0.0;
      for (cnn::int_type k = 0; k < prev.size(); k ++) {
        real_type gradient = delta[j]*prev[k];
#ifdef DEBUG
        printf("update_backward::before l=%d,j=%d,k=%d: weight=%f gradient=%f\n", l,j,k, weights.w[it{l, j, k}].weight, gradient);
#endif
        weights.w[it{l, j, k}].update_weight(gradient); // grad w_jk = delta_j * prev_k
        weights.w[it{l, j, k}].update_bias(delta[j]); // grad b_j = delta_j
#ifdef DEBUG
        printf("update_backward::after l=%d,j=%d,k=%d: weight=%f \n", l,j,k, weights.w[it{l, j, k}].weight);
#endif
      }
    }
    
    return 0;
  }

  std::vector<real_type> values() const {
    std::vector<real_type> neuron_values;
    neuron_values.reserve(neurons.size());
    for (const auto& neuron : neurons) {
      neuron_values.push_back(neuron.value);
    }
    return neuron_values;
  }
};

};

#endif

