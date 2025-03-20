#ifndef WEIGHTS_HPP
#define WEIGHTS_HPP

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

#include <stdio.h> 
#include <stdlib.h> 
#include <cassert>

#include "typedef.hpp"
#include "connector.hpp"
#include "sparse_array.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <limits>
#define NVP(a) BOOST_SERIALIZATION_NVP(a)

using namespace boost::numeric::ublas;

namespace cnn{

struct WeightStats {
    double mean;
    double median;
    double stddev;
    double min;
    double max;
    size_t count;
};

class Weights{

public:
  sparse_array<cnn::Connector> w;

	template<class Ar>
	void serialize(Ar& ar, unsigned){
    //for (const auto& [key, value] : w){
		//	for (int n : key)
    //    ar & NVP( n);
    //  ar & NVP( value );
    //}
    ar & NVP(w);
	}
	bool operator==(Weights const& w_) const{return w_.w.size() == w.size();}

	void save(const std::string& filename) {
			std::ofstream ofs(filename);
			boost::archive::text_oarchive oa(ofs);
			oa << w; // Serialize the unordered_map
	}

	void load(const std::string& filename) {
			std::ifstream ifs(filename);
			boost::archive::text_iarchive ia(ifs);
			ia >> w; // Deserialize into the unordered_map
	}

  // ostream operator
  friend std::ostream& operator<< (std::ostream& stream, const Weights & w_) {

    for (const auto& [key, value] : w_.w){
		for (int n : key)
			std::cout << n << ' ';
		std::cout << " : " << value ;
	}
    return stream;
  }

  WeightStats calculate_weight_statistics() const {
    WeightStats stats{0.0, 0.0, 0.0, std::numeric_limits<double>::max(), 
                     -std::numeric_limits<double>::max(), 0};
    std::vector<double> all_weights;
    all_weights.reserve(w.size());

    // Collect all weights and find min/max
    for (const auto& [key, connector] : w) {
        double weight = connector.weight;
        all_weights.push_back(weight);
        stats.min = std::min(stats.min, weight);
        stats.max = std::max(stats.max, weight);
    }

    size_t n = all_weights.size();
    if (n == 0) {
        return stats;
    }

    // Calculate mean
    double sum = 0.0;
    for (double weight : all_weights) {
        sum += weight;
    }
    stats.mean = sum / n;

    // Calculate median
    std::vector<double> sorted_weights = all_weights;
    std::sort(sorted_weights.begin(), sorted_weights.end());
    if (n % 2 == 0) {
        stats.median = (sorted_weights[n/2 - 1] + sorted_weights[n/2]) / 2.0;
    } else {
        stats.median = sorted_weights[n/2];
    }

    // Calculate standard deviation
    double sum_squared_diff = 0.0;
    for (double weight : all_weights) {
        double diff = weight - stats.mean;
        sum_squared_diff += diff * diff;
    }
    stats.stddev = std::sqrt(sum_squared_diff / n);
    
    stats.count = n;

    return stats;
  }

  // Helper function to print stats
  void print_weight_statistics() const {
    WeightStats stats = calculate_weight_statistics();
    std::cout << "Weight Statistics:\n"
              << "  Count:  " << stats.count << "\n"
              << "  Min:    " << std::fixed << std::setprecision(6) << stats.min << "\n"
              << "  Max:    " << std::fixed << std::setprecision(6) << stats.max << "\n"
              << "  Mean:   " << std::fixed << std::setprecision(6) << stats.mean << "\n"
              << "  Median: " << std::fixed << std::setprecision(6) << stats.median << "\n"
              << "  StdDev: " << std::fixed << std::setprecision(6) << stats.stddev << "\n";
  }

private:
};

};

#endif
