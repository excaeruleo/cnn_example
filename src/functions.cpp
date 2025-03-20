#include "functions.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <random>
namespace cnn{
cnn::real_type linear(const cnn::real_type x){
	return x;
}

cnn::real_type derivative_linear(const cnn::real_type x){
	return 1;
}

cnn::real_type sigmoid(const cnn::real_type x){
	return 1./(1. + exp(-x));
}

cnn::real_type derivative_sigmoid(const cnn::real_type x){
	return sigmoid(x)*(1. - sigmoid(x));
}

cnn::real_type relu(const cnn::real_type x) {
	return std::max(static_cast<real_type>(0.0), x);
}

cnn::real_type derivative_relu(const cnn::real_type x) {
	return x > 0.0 ? 1.0 : 0.0;
}

cnn::real_type cross_entropy_loss(const std::vector<cnn::real_type>& predicted, 
                                   const std::vector<cnn::real_type>& target) {
    const real_type epsilon = static_cast<real_type>(1e-15);  // Small constant to prevent log(0)
    real_type loss = 0.0;
    
    // Ensure vectors have the same size
    if (predicted.size() != target.size()) {
        throw std::runtime_error("Predicted and target vectors must have the same size");
    }

    // Apply softmax to predicted values first
    real_type sum_exp = 0.0;
    std::vector<real_type> softmax_output(predicted.size());
    
    // Find maximum value for numerical stability
    real_type max_val = *std::max_element(predicted.begin(), predicted.end());
    
    // Compute exp of each prediction and sum
    for (size_t i = 0; i < predicted.size(); ++i) {
        softmax_output[i] = std::exp(predicted[i] - max_val);  // Subtract max for numerical stability
        sum_exp += softmax_output[i];
    }
    
    // Normalize to get probabilities
    for (size_t i = 0; i < predicted.size(); ++i) {
        softmax_output[i] /= sum_exp;
        // Clip values to prevent log(0), using proper type casting
        softmax_output[i] = std::max(epsilon, 
                                   std::min(static_cast<real_type>(1.0) - epsilon, 
                                          softmax_output[i]));
    }

    // Compute cross-entropy loss
    for (size_t i = 0; i < predicted.size(); ++i) {
        loss -= target[i] * std::log(softmax_output[i]);
    }

#ifndef DEBUG
    std::cout << "Cross Entropy Loss Calculation:\n";
    std::cout << "Predicted (after softmax): ";
    for (const auto& p : softmax_output) std::cout << p << " ";
    std::cout << "\nTarget: ";
    for (const auto& t : target) std::cout << t << " ";
    std::cout << "\nLoss: " << loss << "\n";
#endif

    return loss;
}

cnn::real_type mse_loss(const std::vector<real_type>& predicted, 
                           const std::vector<real_type>& target) {
        if (predicted.size() != target.size()) {
            throw std::runtime_error("Predicted and target vectors must have the same size");
        }
        
        real_type sum = 0.0;
        for (size_t i = 0; i < predicted.size(); ++i) {
            real_type diff = predicted[i] - target[i];
            sum += diff * diff;
        }
        return sum / predicted.size();
    }

cnn::real_type mae_loss(const std::vector<real_type>& predicted, 
                           const std::vector<real_type>& target) {
        if (predicted.size() != target.size()) {
            throw std::runtime_error("Predicted and target vectors must have the same size");
        }
        
        real_type sum = 0.0;
        for (size_t i = 0; i < predicted.size(); ++i) {
            sum += std::abs(predicted[i] - target[i]);
        }
        return sum / predicted.size();
    }

cnn::real_type huber_loss(const std::vector<real_type>& predicted, 
                             const std::vector<real_type>& target,
                             real_type delta) {
        if (predicted.size() != target.size()) {
            throw std::runtime_error("Predicted and target vectors must have the same size");
        }
        
        real_type sum = 0.0;
        for (size_t i = 0; i < predicted.size(); ++i) {
            real_type diff = std::abs(predicted[i] - target[i]);
            if (diff <= delta) {
                sum += 0.5 * diff * diff;
            } else {
                sum += delta * diff - 0.5 * delta * delta;
            }
        }
        return sum / predicted.size();
    }

cnn::real_type binary_cross_entropy_loss(const std::vector<real_type>& predicted, 
                                           const std::vector<real_type>& target) {
        const real_type epsilon = static_cast<real_type>(1e-15);
        if (predicted.size() != target.size()) {
            throw std::runtime_error("Predicted and target vectors must have the same size");
        }
        
        real_type sum = 0.0;
        for (size_t i = 0; i < predicted.size(); ++i) {
            real_type p = std::max(epsilon, 
                                 std::min(static_cast<real_type>(1.0) - epsilon, 
                                        predicted[i]));
            sum -= target[i] * std::log(p) + 
                   (static_cast<real_type>(1.0) - target[i]) * std::log(static_cast<real_type>(1.0) - p);
        }
        return sum / predicted.size();
    }

cnn::real_type hinge_loss(const std::vector<real_type>& predicted, 
                             const std::vector<real_type>& target) {
        if (predicted.size() != target.size()) {
            throw std::runtime_error("Predicted and target vectors must have the same size");
        }
        
        real_type sum = 0.0;
        for (size_t i = 0; i < predicted.size(); ++i) {
            sum += std::max(static_cast<real_type>(0.0), 
                          static_cast<real_type>(1.0) - predicted[i] * target[i]);
        }
        return sum / predicted.size();
    }

cnn::real_type kl_divergence_loss(const std::vector<real_type>& predicted, 
                                     const std::vector<real_type>& target) {
        const real_type epsilon = static_cast<real_type>(1e-15);
        if (predicted.size() != target.size()) {
            throw std::runtime_error("Predicted and target vectors must have the same size");
        }
        
        real_type sum = 0.0;
        for (size_t i = 0; i < predicted.size(); ++i) {
            real_type p = std::max(epsilon, 
                                 std::min(static_cast<real_type>(1.0) - epsilon, 
                                        predicted[i]));
            real_type t = std::max(epsilon, 
                                 std::min(static_cast<real_type>(1.0) - epsilon, 
                                        target[i]));
            sum += t * std::log(t / p);
        }
        return sum;
    }

cnn::real_type softmax(const cnn::real_type x) {
    // Note: This is a single-value version. 
    // Typically softmax is applied to a vector of values.
    // This implementation is for compatibility with the existing activation function interface
    return std::exp(x);  // The normalization happens in the layer
}

cnn::real_type derivative_softmax(const cnn::real_type x) {
    // The derivative of softmax is handled specially in the backpropagation
    // because it depends on all outputs, not just one value
    // This is a simplified version for the single-value case
    real_type s = softmax(x);
    return s * (1.0 - s);
}

// Optional: Vector version of softmax (more numerically stable)
std::vector<cnn::real_type> softmax_vector(const std::vector<cnn::real_type>& x) {
    std::vector<real_type> output(x.size());
    
    // Find maximum value for numerical stability
    real_type max_val = *std::max_element(x.begin(), x.end());
    
    // Compute exp of each value and sum
    real_type sum_exp = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        output[i] = std::exp(x[i] - max_val);  // Subtract max for numerical stability
        sum_exp += output[i];
    }
    
    // Normalize to get probabilities
    for (size_t i = 0; i < x.size(); ++i) {
        output[i] /= sum_exp;
    }
    
    return output;
}

cnn::real_type kaiming_uniform_init(int fan_in) {
    // He/Kaiming initialization for ReLU activation
    real_type limit = std::sqrt(2.0 / fan_in);
    
    // Create a random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<real_type> dis(-limit, limit);
    
    return dis(gen);
}

cnn::real_type xavier_uniform_init(int fan_in, int fan_out) {
    // Glorot/Xavier initialization
    real_type limit = std::sqrt(6.0 / (fan_in + fan_out));
    
    // Create a random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<real_type> dis(-limit, limit);
    
    return dis(gen);
}

std::vector<std::vector<real_type>> orthogonal_init(int rows, int cols) {
    // Generate a random matrix
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::normal_distribution<real_type> dist(0.0, 1.0);
    
    // Create a random matrix A
    std::vector<std::vector<real_type>> A(rows, std::vector<real_type>(cols));
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            A[i][j] = dist(gen);
        }
    }
    
    // Perform QR decomposition using Gram-Schmidt process
    std::vector<std::vector<real_type>> Q(rows, std::vector<real_type>(cols, 0.0));
    
    for(int j = 0; j < cols; ++j) {
        // Get the j-th column of A
        std::vector<real_type> v(rows);
        for(int i = 0; i < rows; ++i) {
            v[i] = A[i][j];
        }
        
        // Subtract projections onto previous vectors
        for(int k = 0; k < j; ++k) {
            // Calculate dot product
            real_type dot = 0.0;
            for(int i = 0; i < rows; ++i) {
                dot += v[i] * Q[i][k];
            }
            
            // Subtract projection
            for(int i = 0; i < rows; ++i) {
                v[i] -= dot * Q[i][k];
            }
        }
        
        // Normalize the vector
        real_type norm = 0.0;
        for(int i = 0; i < rows; ++i) {
            norm += v[i] * v[i];
        }
        norm = std::sqrt(norm);
        
        if(norm > 1e-8) {  // Avoid division by zero
            for(int i = 0; i < rows; ++i) {
                Q[i][j] = v[i] / norm;
            }
        }
    }
    
    // Scale the matrix (optional)
    real_type gain = std::sqrt(2.0);  // Similar to ReLU gain
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            Q[i][j] *= gain;
        }
    }
    
    return Q;
}

}
