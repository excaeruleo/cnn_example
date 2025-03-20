#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

// Example data structure
struct MyData {
    int id;
    std::string name;
    std::vector<int> values;

    // Serialization support
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & id;
        ar & name;
        ar & values;
    }
};

// Function to serialize and compress data
void saveCompressed(const MyData& data, const std::string& filename) {
    std::ofstream ofs(filename, std::ios_base::binary);
    if (!ofs) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    // Create a filtering stream that compresses data
    boost::iostreams::filtering_ostream out;
    out.push(boost::iostreams::zlib_compressor()); // Use zlib compression
    out.push(ofs); // Output to file

    // Serialize the data to the filtering stream
    boost::archive::text_oarchive oa(out);
    oa << data;

    std::cout << "Data has been serialized and compressed to " << filename << std::endl;
}

// Function to decompress and deserialize data
void loadCompressed(MyData& data, const std::string& filename) {
    std::ifstream ifs(filename, std::ios_base::binary);
    if (!ifs) {
        std::cerr << "Failed to open file for reading." << std::endl;
        return;
    }

    // Create a filtering stream that decompresses data
    boost::iostreams::filtering_istream in;
    in.push(boost::iostreams::zlib_decompressor()); // Use zlib decompression
    in.push(ifs); // Input from file

    // Deserialize the data from the filtering stream
    boost::archive::text_iarchive ia(in);
    ia >> data;

    std::cout << "Data has been decompressed and deserialized from " << filename << std::endl;
}

int main() {
    // Example data to serialize
    MyData originalData = {1, "Test Data", {1, 2, 3, 4, 5}};

    // File to save the compressed data
    std::string filename = "compressed_data.dat";

    // Save the data with compression
    saveCompressed(originalData, filename);

    // Load the data with decompression
    MyData loadedData;
    loadCompressed(loadedData, filename);

    // Display the loaded data
    std::cout << "Loaded Data: " << loadedData.id << ", " << loadedData.name << ", Values: ";
    for (int value : loadedData.values) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}

