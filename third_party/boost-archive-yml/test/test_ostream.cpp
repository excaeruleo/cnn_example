#include "../yml_oarchive.hpp"
#include "../yml_iarchive.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp> // serialize boost::optional

#include<fstream>

#define NVP(a) BOOST_SERIALIZATION_NVP(a) 

struct custom{
	double x; int y; unsigned z;
	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar & NVP(x) & NVP(y) & NVP(z);
	}
	bool operator==(custom const& o) const{return o.x==x and o.y==y and o.z==z;}
};


int main(){
	std::string salute = "Hello, World!";
	int num_people = 12;
	std::pair<double, double> square = {42, 84};
	boost::optional<double> tallest = 1.23;
	std::vector<std::string>  pets = {"Bob", "Rex", "Tom", "Jerry"};
	std::array<double, 3> trio = {1.1, 2.2, 3.3};
	std::vector<std::vector<std::string>> nicknames = {
		{"Aaron", "Erin", "Ronnie", "Ron"},
		{"Adam", "Edie", "Ade"},
		{"Albert", "Bert", "Al"},
		{"Alfred", "Freddy", "Al", "Fred"},
		{"Wilfred", "Will", "Willie", "Fred"}
	};
	std::set<double> bag = {3, 2, 1};
	custom C{1.1, -2, 3};

	{
		std::ofstream ofs{"simple.yml"};
		boost::archive::yml_oarchive yoa{ofs};
		yoa
			<< NVP(salute)
			<< NVP(num_people) 
			<< NVP(square)
			<< NVP(tallest)
			<< NVP(pets)
			<< NVP(trio)
			<< NVP(nicknames)
			<< NVP(bag)
			<< NVP(C)
		;
	}
}
