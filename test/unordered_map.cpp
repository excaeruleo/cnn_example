#include <vector>
#include <unordered_map>
#include <numeric>
#include <iostream>

// https://stackoverflow.com/questions/4306/what-is-the-best-way-to-create-a-sparse-array-in-c
using index_type = std::vector<int>;

struct index_hash {
    std::size_t operator()(index_type const& i) const noexcept {
        // Like boost::hash_combine; there might be some caveats, see
        // <https://stackoverflow.com/a/50978188/1968>
        auto const hash_combine = [](auto seed, auto x) {
            return std::hash<int>()(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };
        return std::accumulate(i.begin() + 1, i.end(), i[0], hash_combine);
    }
};

template <typename T>
using sparse_array = std::unordered_map<index_type, T, index_hash>;

int main() {
    using i = index_type;

    auto x = sparse_array<int>();
    x[i{1, 2, 3}] = 42;
    x[i{4, 3, 2}] = 23;

    std::cout << x[i{1, 2, 3}] + x[i{4, 3, 2}] << '\n'; // 65

    auto y = sparse_array<int>();
    y[i{1, 2}] = 44;
    y[i{4, 3}] = 22;

    std::cout << y[i{1, 2}] + y[i{4, 3}] << '\n'; // 65

    std::cout << "\nIterate and print key-value pairs using C++17 structured binding:\n";
    for (const auto& [key, value] : y){
			for (int n : key)
        std::cout << n << ' ';
      std::cout << " : " << value << std::endl;
    }
}
