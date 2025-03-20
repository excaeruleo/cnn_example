#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>

void save(const std::unordered_map<int, std::string>& map, const std::string& filename) {
    std::ofstream ofs(filename);
    boost::archive::text_oarchive oa(ofs);
    oa << map; // Serialize the unordered_map
}

void load(std::unordered_map<int, std::string>& map, const std::string& filename) {
    std::ifstream ifs(filename);
    boost::archive::text_iarchive ia(ifs);
    ia >> map; // Deserialize into the unordered_map
}

int main() {
    // Create an unordered_map and serialize it
    std::unordered_map<int, std::string> map = {
        {1, "one"},
        {2, "two"},
        {3, "three"}
    };
    
    save(map, "map_data.txt");

    // Deserialize into another unordered_map
    std::unordered_map<int, std::string> new_map;
    load(new_map, "map_data.txt");

    // Output the contents of the deserialized unordered_map
    for (const auto& pair : new_map) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}
