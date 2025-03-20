#ifndef SPARSE_ARRAY_HPP
#define SPARSE_ARRAY_HPP

#include <vector>
#include <unordered_map>
#include <numeric>
#include "typedef.hpp"

using weight_index_type = std::vector<cnn::int_type>;

struct index_hash {
    std::size_t operator()(weight_index_type const& i) const noexcept {
        // Like boost::hash_combine; there might be some caveats, see
        // <https://stackoverflow.com/a/50978188/1968>
        auto const hash_combine = [](auto seed, auto x) {
            return std::hash<int>()(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };
        return std::accumulate(i.begin() + 1, i.end(), i[0], hash_combine);
    }
};

template <typename T>
using sparse_array = std::unordered_map<weight_index_type, T, index_hash>;

#endif
