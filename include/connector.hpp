#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <algorithm>
#include <cmath>
#include <ostream>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include "typedef.hpp"
#include "functions.hpp"

namespace cnn {

enum class InitType {
  KAIMING,
  XAVIER,
  ORTHOGONAL
};

// Connector stores the weight information and information to update weight
class Connector {
public:
  // next layer number: l
  int layer;
  // each element in this vector corresponds to index of neuron from previous layer
  int input_index;
  // each element in this vector corresponds to index of neuron from next layer
  int output_index;
  // each element in this vector corresponds to weight multiplied to input value of neuron from previous layer
  real_type weight;
  // each element in this vector corresponds to bias added to w_i x_i
  real_type bias;
  // learning rate to adjust learning speed
  real_type learning_rate;
  // momentum to capture second order rate of change of learning rate
  real_type momentum;
  // learning rate decay rate
  real_type decay;

  // for lr adjustment
  real_type old_gradient;
  int_type  iter;

private:
  InitType init_type;
  static inline std::vector<std::vector<real_type>> orthogonal_matrix;
  static inline int orthogonal_matrix_index = 0;

private:
  friend class boost::serialization::access;

  template <typename Ar>
  void serialize(Ar& ar, unsigned /*version*/)
  {
    ar &
      BOOST_SERIALIZATION_NVP(layer) &
      BOOST_SERIALIZATION_NVP(input_index) &
      BOOST_SERIALIZATION_NVP(output_index) &
      BOOST_SERIALIZATION_NVP(weight) &
      BOOST_SERIALIZATION_NVP(bias) &
      BOOST_SERIALIZATION_NVP(learning_rate) &
      BOOST_SERIALIZATION_NVP(momentum) &
      BOOST_SERIALIZATION_NVP(decay);
  }

public:
  bool operator==(Connector const& c_) const{
   return c_.weight        == weight
       && c_.layer         == layer
       && c_.input_index   == input_index
       && c_.output_index  == output_index
       && c_.bias          == bias
       && c_.learning_rate == learning_rate
       && c_.momentum      == momentum;
  }

  // Modify the constructors to include initialization type
  Connector(): layer(1), init_type(InitType::KAIMING) {
     default_initialize();
  }

  Connector(int layer_, int input_index_, int output_index_,
           int fan_in = 784, int fan_out = 256,
           InitType init = InitType::KAIMING):
      layer(layer_),
      input_index(input_index_),
      output_index(output_index_),
      init_type(init)
  {
      default_initialize();
      switch(init_type) {
          // for relu filter
          case InitType::KAIMING:
              weight = kaiming_uniform_init(fan_in);
              break;
          // for tanh/sigmoid filter
          case InitType::XAVIER:
              weight = xavier_uniform_init(fan_in, fan_out);
              break;
          // for orthogonal filter
          case InitType::ORTHOGONAL:
              // Initialize orthogonal matrix if not already done
              if (orthogonal_matrix.empty() ||
                  orthogonal_matrix.size() != fan_out ||
                  orthogonal_matrix[0].size() != fan_in) {
                  orthogonal_matrix = orthogonal_init(fan_out, fan_in);
                  orthogonal_matrix_index = 0;
              }
              // Use the next value from the orthogonal matrix
              weight = orthogonal_matrix[output_index][input_index];
              break;
      }
  }

  void update_bias(real_type gradient){
    // Gradient clipping to prevent exploding gradient
    const real_type clip_gradient = 0.01f;
    gradient = std::min(std::max(gradient, -clip_gradient), clip_gradient);

    bias -= gradient * learning_rate;

    // Clip bias values to prevent them from growing too large
    const real_type clip_bias = 0.01f;
    bias = std::min(std::max(bias, -clip_bias), clip_bias);
  }

  void update_weight(real_type gradient){
    if(std::abs(gradient) < 1.e-10){
#ifdef DEBUG
      std::cout << "gradient = " << gradient << std::endl;
#endif
      return;
    }

    // Gradient clipping to prevent exploding gradient
    const real_type clip_gradient = 1.0;
    gradient = std::min(std::max(gradient, -clip_gradient), clip_gradient);

    weight -= gradient * learning_rate;
    if(iter == 0)
      old_gradient = gradient;
    else{
      if(gradient * old_gradient > 0)
        learning_rate += momentum*std::abs(gradient + old_gradient) / 2;
    }
    learning_rate /= (1 + decay*iter);
    iter ++;

    const real_type clip_weight = 1.0;
    weight = std::min(std::max(weight, -clip_weight), clip_weight);
    //if (std::isnan(weight) || std::isinf(weight)) {
    //    std::cout << "Warning: Invalid weight detected after update!" << std::endl;
    //    exit(-1);
    //}
  }

  void reset(const real_type lr, const real_type m=0.01, const real_type d=0.001) {
    learning_rate = lr;
    momentum = m;
    decay = d;
    iter = 0;
  }

  // ostream operator
  friend std::ostream& operator<<(std::ostream& out, const Connector& n)
  {
    return out <<
      "(" << n.layer << "," << n.input_index << "," << n.output_index <<  "): "
      << n.weight << ' ' << n.bias << ' ' << n.learning_rate << ' ' <<
      n.momentum  << "\n";
  }

private:
  void default_initialize(){
    // Default to Kaiming initialization
    weight = kaiming_uniform_init(784);
    bias = 0.;
    learning_rate = 0.1f;
    momentum = 0.01f;
    decay = 0.001f; // reduce the learning rate by half after 1000 iterations
    //momentum = 0.0;
    //decay = 0.0;
    old_gradient = -1000;
    iter = 0;
  }
};

}  // namespace cnn

#endif  // CONNECTOR_HPP
