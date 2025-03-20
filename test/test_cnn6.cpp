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
#include <iomanip>
#include <numeric>
#include <random>

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

  int get_output_label() const {
    // Get the output layer values
    const auto& output = this->get_output();
    
    // Find the index of the maximum value in the output layer
    // This index corresponds to the predicted digit (0-9)
    auto max_it = std::max_element(output.begin(), output.end());
    return std::distance(output.begin(), max_it);
  }

};

bool simple_neural_network::initialize() {
    std::vector<int> layer_sizes = {784, 256, 10};  // Define layer sizes
    n_layers = layer_sizes.size();
    epoch_frequency = 200;
    // Create layers
    layers.clear();
    for (size_t i = 0; i < layer_sizes.size(); ++i) {
        cnn::Layer layer(layer_sizes[i]);
        layer.layer_name = "layer_" + std::to_string(i);
        layers.push_back(layer);
    }
    
    expected = cnn::Layer(layer_sizes.back());

    // Initialize weights between layers using Adam
    using it = weight_index_type;
    for (size_t l = 1; l < layer_sizes.size(); ++l) {
        for (int j = 0; j < layer_sizes[l]; ++j) {
            for (int k = 0; k < layer_sizes[l-1]; ++k) {
                weights.w[it{static_cast<int>(l), j, k}] = 
                    cnn::Connector(l, j, k);  // Pass layer sizes
            }
        }
    }

    cost_function = cnn::cross_entropy_loss;
    layers[0].set_optimizer(cnn::optimizer("relu", cnn::relu, cnn::derivative_relu));
    layers[1].set_optimizer(cnn::optimizer("relu", cnn::relu, cnn::derivative_relu));
    //layers.back().set_optimizer(cnn::optimizer("softmax", cnn::softmax, cnn::derivative_softmax));

    return true;
}

/* reset the input to the image file pixel value */
bool 
simple_neural_network::reset_input(const std::vector<std::vector<cnn::real_type>> & input, const int index){
#ifdef DEBUG
	std::cout << layers[0].size() << " " << input[index].size();
#endif
	assert(this->layers[0].size() == input[index].size());
	for(cnn::int_type i = 0; i < this->layers[0].size(); i ++)
		this->layers[0][i]=input[index][i];
	return true;
}
/* reset the out to the expected digit */
bool
simple_neural_network::reset_expected(const int & index){
	assert(this->expected.size() > index);
	for(cnn::int_type i = 0; i < this->expected.size(); i ++)
		this->expected[i]=0;
        this->expected[index]=1;
#ifdef DEBUG
	std::cout << "expected = " << this->expected[index] << std::endl;
#endif
	return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " --train|--resume|--apply" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string datafilename, labelfilename;

    std::vector<std::vector<cnn::real_type>> pixelValues;
    std::vector<int> labels;

    cnn::int_type n_images = 1000;

    try {
        if(mode == "--train"){
            datafilename = "test/testinput/train-images-idx3-ubyte.gz";
            labelfilename = "test/testinput/train-labels-idx1-ubyte.gz"; 
            // Read the MNIST file
            cnn::readMNISTBinaryFilesSubset(datafilename, labelfilename, pixelValues, labels, n_images);
        } else if (mode == "--resume"){
            datafilename = "test/testinput/train-images-idx3-ubyte.gz";
            labelfilename = "test/testinput/train-labels-idx1-ubyte.gz"; 
            // Read the MNIST file
            cnn::readMNISTBinaryFilesSubset(datafilename, labelfilename, pixelValues, labels, n_images);
        } else if (mode == "--apply"){
            datafilename = "test/testinput/t10k-images-idx3-ubyte.gz";
            labelfilename = "test/testinput/t10k-labels-idx1-ubyte.gz"; 
            // Read the MNIST file
            cnn::readMNISTBinaryFilesSubset(datafilename, labelfilename, pixelValues, labels, n_images);
        } else {
            std::cerr << "Invalid mode. Use --train or --apply." << std::endl;
            return 1;
        }

        // Output confirmation
        std::cout << "Successfully read " << pixelValues.size() << " images with "
                  << pixelValues[0].size() << " pixels each and " << labels.size() 
                  << " labels." << std::endl;
#ifdef DEBUG
        // Debug output for first digit
        std::cout << "\nFirst digit (label: " << labels[0] << "):\n";
        for (int i = 0; i < 28; i++) {
            for (int j = 0; j < 28; j++) {
                // Print character based on pixel intensity
                int pixel = pixelValues[0][i * 28 + j];
                char c = pixel == 0 ? ' ' : (pixel < 128 ? '.' : '#');
                std::cout << c << c; // Print twice for better aspect ratio
            }
            std::cout << '\n';
        }
        std::cout << "\nRaw pixel values of first digit: \n";
        for(int i = 0; i < 28; i++){
            for (int j = 0; j < 28; j++) {
               std::cout << std::setw(4) << pixelValues[0][i*28+j];
            }
            std::cout << "\n";
        }
        std::cout << "\n\n";
#endif
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Normalize pixel values to [0,1]
    for (auto& image : pixelValues) {
        for (auto& pixel : image) {
            pixel = static_cast<cnn::real_type>(pixel) / 255.0;
        }
    }

#ifdef DEBUG
    // Debug: Print first few normalized pixels
    std::cout << "First few normalized digit: ";
    for (int i = 0; i < 28; ++i) {
        for (int j = 0; j < 28; ++j) {
            std::cout << std::setw(10) << std::fixed << std::setprecision(3) << pixelValues[0][i*28+j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
#endif
    simple_neural_network nn;

    if (mode == "--train" || mode == "--resume") {
        if (mode == "--train") {
            nn.initialize();
        } else {
            std::ifstream ifs{"trained.yml"};
            boost::archive::yml_iarchive yia{ifs};
            yia >> NVP(nn);
        }
        //nn.set_optimizer(cnn::optimizer("sigmoid", cnn::sigmoid, cnn::derivative_sigmoid));
        //nn.set_optimizer(cnn::optimizer("relu", cnn::relu, cnn::derivative_relu));

        for(cnn::int_type idx = 0; idx < pixelValues.size(); idx++){
            std::cout << "Processing image #" << idx << std::endl;
            std::cout << "labels[idx] = " << labels[idx] << std::endl;
            nn.reset_input(pixelValues, idx);
            nn.reset_expected(labels[idx]);
            nn.reset_learning_parameters();
#ifdef DEBUG
            std::cout << nn << std::endl;
#endif
            nn.update();
        }

        // Save final model
        std::ofstream ofs{"trained.yml"};
        boost::archive::yml_oarchive yoa{ofs};
        yoa << NVP(nn);
    } else if (mode == "--apply") {
        std::ifstream ifs{"trained.yml"};
        boost::archive::yml_iarchive yia{ifs};
        yia >> NVP(nn);
#ifndef DEBUG
        std::cout << "nn = " << nn << std::endl;
#endif
        std::cout << "nn.print_weight_statistics() = " << std::endl;
        nn.print_weight_statistics();

        // Variables for tracking performance
        int correct_predictions = 0;
        std::vector<int> confusion_matrix(100, 0);  // 10x10 matrix flattened

        // Process each image
        for (cnn::int_type i = 0; i < pixelValues.size(); i++) {
            nn.reset_input(pixelValues, i);
            nn.update_forward();
            int predicted_label = nn.get_output_label();
            int true_label = labels[i];
            
            // Update confusion matrix
            confusion_matrix[true_label * 10 + predicted_label]++;
            
            if (predicted_label == true_label) {
                correct_predictions++;
            }
            
            std::cout << "Image #" << i << " predicted: " << predicted_label 
                      << " actual: " << true_label << std::endl;
        }

        // Print summary
        std::cout << "\n=== Performance Summary ===\n";
        std::cout << "Total images: " << pixelValues.size() << std::endl;
        std::cout << "Correct predictions: " << correct_predictions << std::endl;
        double accuracy = static_cast<double>(correct_predictions) / pixelValues.size() * 100;
        std::cout << "Accuracy: " << accuracy << "%\n\n";

        // Print confusion matrix
        std::cout << "Confusion Matrix:\n";
        std::cout << "   ";
        for (int i = 0; i < 10; i++) std::cout << std::setw(4) << i;
        std::cout << " <- Predicted\n";
        
        for (int i = 0; i < 10; i++) {
            std::cout << std::setw(2) << i << " ";
            for (int j = 0; j < 10; j++) {
                std::cout << std::setw(4) << confusion_matrix[i * 10 + j];
            }
            std::cout << std::endl;
        }
        std::cout << "^\nActual\n";

        // Print per-digit accuracy
        std::cout << "\nPer-digit Accuracy:\n";
        for (int i = 0; i < 10; i++) {
            int digit_total = 0;
            for (int j = 0; j < 10; j++) {
                digit_total += confusion_matrix[i * 10 + j];
            }
            double digit_accuracy = static_cast<double>(confusion_matrix[i * 10 + i]) / digit_total * 100;
            std::cout << "Digit " << i << ": " << digit_accuracy << "%\n";
        }

    } else {
        std::cerr << "Invalid mode. Use --train, --resume, or --apply." << std::endl;
        return 1;
    }

    return 0;
}
