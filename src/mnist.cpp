#include "mnist.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <zlib.h> // For gzFile and gzopen
#include <random>
#include <algorithm>

namespace cnn{
	// Subroutine to read MNIST file and store values in vectors
	void readMNISTFile(const std::string& filename, 
										 std::vector<std::vector<real_type>>& pixelValues, 
										 std::vector<int>& labels) {
			// Open the input file
			std::ifstream inputFile(filename);
			if (!inputFile.is_open()) {
					throw std::runtime_error("Error: Could not open the file " + filename);
			}

			std::string line;
			while (std::getline(inputFile, line)) {
					std::stringstream ss(line);
					std::vector<int> image(784); // Temporary storage for pixel values
					int label;

					// Read pixel values (784 numbers)
					for (int i = 0; i < 784; ++i) {
							if (!(ss >> image[i])) {
									throw std::runtime_error("Error: Invalid file format");
							}
							// Consume comma delimiter if present
							if (ss.peek() == ',') {
									ss.ignore();
							}
					}

					// Read the label (last number on the line)
					if (!(ss >> label)) {
							throw std::runtime_error("Error: Invalid file format");
					}

					// Store the parsed values in the vectors
					pixelValues.push_back(std::vector<real_type>(image.begin(), image.end()));
					labels.push_back(label);
			}

			// Close the file
			inputFile.close();

			// Check if the data size is as expected
			if (pixelValues.size() != 1000 || labels.size() != 1000) {
					throw std::runtime_error("Error: File does not contain exactly 1000 images");
			}
	}

	// New function to read binary files
	void readMNISTBinaryFiles(const std::string& imagesFilename, 
							  const std::string& labelsFilename, 
							  std::vector<std::vector<real_type>>& pixelValues, 
							  std::vector<int>& labels) {
			// Open the image file
			gzFile imageFile = gzopen(imagesFilename.c_str(), "rb");
			if (!imageFile) {
					throw std::runtime_error("Error: Could not open the image file " + imagesFilename);
			}

			// Open the label file
			gzFile labelFile = gzopen(labelsFilename.c_str(), "rb");
			if (!labelFile) {
					gzclose(imageFile);
					throw std::runtime_error("Error: Could not open the label file " + labelsFilename);
			}

			// Read the magic number and number of images
			int magicNumber = 0;
			int numberOfImages = 0;
			int numberOfRows = 0;
			int numberOfColumns = 0;

			gzread(imageFile, &magicNumber, sizeof(magicNumber));
			magicNumber = __builtin_bswap32(magicNumber);
			if (magicNumber != 2051) {
					gzclose(imageFile);
					gzclose(labelFile);
					throw std::runtime_error("Error: Invalid magic number in image file");
			}

			gzread(imageFile, &numberOfImages, sizeof(numberOfImages));
			numberOfImages = __builtin_bswap32(numberOfImages);

			gzread(imageFile, &numberOfRows, sizeof(numberOfRows));
			numberOfRows = __builtin_bswap32(numberOfRows);

			gzread(imageFile, &numberOfColumns, sizeof(numberOfColumns));
			numberOfColumns = __builtin_bswap32(numberOfColumns);

			// Read the magic number and number of labels
			int labelMagicNumber = 0;
			int numberOfLabels = 0;

			gzread(labelFile, &labelMagicNumber, sizeof(labelMagicNumber));
			labelMagicNumber = __builtin_bswap32(labelMagicNumber);
			if (labelMagicNumber != 2049) {
					gzclose(imageFile);
					gzclose(labelFile);
					throw std::runtime_error("Error: Invalid magic number in label file");
			}

			gzread(labelFile, &numberOfLabels, sizeof(numberOfLabels));
			numberOfLabels = __builtin_bswap32(numberOfLabels);

			if (numberOfImages != numberOfLabels) {
					gzclose(imageFile);
					gzclose(labelFile);
					throw std::runtime_error("Error: Number of images does not match number of labels");
			}

			// Read the image and label data
			pixelValues.resize(numberOfImages, std::vector<real_type>(numberOfRows * numberOfColumns));
			labels.resize(numberOfLabels);

			for (int i = 0; i < numberOfImages; ++i) {
					for (int j = 0; j < numberOfRows * numberOfColumns; ++j) {
							unsigned char pixel = 0;
							gzread(imageFile, &pixel, sizeof(pixel));
							pixelValues[i][j] = static_cast<int>(pixel);
					}

					unsigned char label = 0;
					gzread(labelFile, &label, sizeof(label));
					labels[i] = static_cast<int>(label);
			}

			// Close the files
			gzclose(imageFile);
			gzclose(labelFile);
	}

	void readMNISTBinaryFilesSubset(const std::string& imagesFilename, 
								   const std::string& labelsFilename, 
								   std::vector<std::vector<real_type>>& pixelValues, 
								   std::vector<int>& labels,
								   int subsetSize,
								   bool randomAccess) {
			// Open the image file
			gzFile imageFile = gzopen(imagesFilename.c_str(), "rb");
			if (!imageFile) {
					throw std::runtime_error("Error: Could not open the image file " + imagesFilename);
			}

			// Open the label file
			gzFile labelFile = gzopen(labelsFilename.c_str(), "rb");
			if (!labelFile) {
					gzclose(imageFile);
					throw std::runtime_error("Error: Could not open the label file " + labelsFilename);
			}

			// Read headers
			int magicNumber = 0, numberOfImages = 0, numberOfRows = 0, numberOfColumns = 0;
			
			gzread(imageFile, &magicNumber, sizeof(magicNumber));
			magicNumber = __builtin_bswap32(magicNumber);
			if (magicNumber != 2051) {
					gzclose(imageFile);
					gzclose(labelFile);
					throw std::runtime_error("Error: Invalid magic number in image file");
			}

			gzread(imageFile, &numberOfImages, sizeof(numberOfImages));
			numberOfImages = __builtin_bswap32(numberOfImages);
			gzread(imageFile, &numberOfRows, sizeof(numberOfRows));
			numberOfRows = __builtin_bswap32(numberOfRows);
			gzread(imageFile, &numberOfColumns, sizeof(numberOfColumns));
			numberOfColumns = __builtin_bswap32(numberOfColumns);

			// Read label file header
			int labelMagicNumber = 0, numberOfLabels = 0;
			gzread(labelFile, &labelMagicNumber, sizeof(labelMagicNumber));
			labelMagicNumber = __builtin_bswap32(labelMagicNumber);
			if (labelMagicNumber != 2049) {
					gzclose(imageFile);
					gzclose(labelFile);
					throw std::runtime_error("Error: Invalid magic number in label file");
			}

			gzread(labelFile, &numberOfLabels, sizeof(numberOfLabels));
			numberOfLabels = __builtin_bswap32(numberOfLabels);

			if (numberOfImages != numberOfLabels) {
					gzclose(imageFile);
					gzclose(labelFile);
					throw std::runtime_error("Error: Number of images does not match number of labels");
			}

			// Ensure N is not larger than the available images
			subsetSize = std::min(subsetSize, numberOfImages);

			if (randomAccess) {
					// Generate random indices
					std::vector<int> indices(numberOfImages);
					std::iota(indices.begin(), indices.end(), 0); // Fill with 0, 1, 2, ..., numberOfImages-1
					
					std::random_device rd;
					std::mt19937 gen(rd());
					std::shuffle(indices.begin(), indices.end(), gen);

					// Resize output vectors
					pixelValues.resize(subsetSize, std::vector<real_type>(numberOfRows * numberOfColumns));
					labels.resize(subsetSize);

					// Buffer for one image
					std::vector<unsigned char> imageBuffer(numberOfRows * numberOfColumns);
					std::vector<unsigned char> labelBuffer(numberOfImages);

					// Read all labels at once
					gzread(labelFile, labelBuffer.data(), numberOfImages);

					// Read and store selected images
					for (int i = 0; i < subsetSize; ++i) {
							// Seek to the correct position in the image file
							gzseek(imageFile, 16 + indices[i] * (numberOfRows * numberOfColumns), SEEK_SET);
							
							// Read the image
							gzread(imageFile, imageBuffer.data(), numberOfRows * numberOfColumns);
							
							// Convert and store pixel values
							for (int j = 0; j < numberOfRows * numberOfColumns; ++j) {
									pixelValues[i][j] = static_cast<int>(imageBuffer[j]);
							}

							// Store the corresponding label
							labels[i] = static_cast<int>(labelBuffer[indices[i]]);
					}
			} else {
					// Pre-allocate vectors for sequential reading
					pixelValues.resize(subsetSize, std::vector<real_type>(numberOfRows * numberOfColumns));
					labels.resize(subsetSize);

					// Read first subsetSize images sequentially
					for (int i = 0; i < subsetSize && i < numberOfImages; ++i) {
							// Read image pixels
							std::vector<unsigned char> imageBuffer(numberOfRows * numberOfColumns);
							gzread(imageFile, imageBuffer.data(), numberOfRows * numberOfColumns);
							
							// Convert and store pixel values
							for (int j = 0; j < numberOfRows * numberOfColumns; ++j) {
									pixelValues[i][j] = static_cast<real_type>(imageBuffer[j]);
							}
							
							// Read and store label
							unsigned char label;
							gzread(labelFile, &label, sizeof(label));
							labels[i] = static_cast<int>(label);
					}
			}

			// Close the files
			gzclose(imageFile);
			gzclose(labelFile);
	}
}
