#include "../yml_oarchive.hpp"
#include "../yml_iarchive.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/optional.hpp> // serialize boost::optional

#include<fstream>
#include<iostream>

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
	std::string salute;
	int num_people;
	std::pair<double, double> square;
	boost::optional<double> tallest;
	std::vector<std::string>  pets;
	std::array<double, 3> trio;
	std::vector<std::vector<std::string>> nicknames;
	std::set<double> bag;
	custom C;

	{
		std::ifstream ifs{"simple.yml"};
		boost::archive::yml_iarchive yia{ifs};
		yia 
			>> NVP(salute)
			>> NVP(num_people)
			>> NVP(square)
			>> NVP(tallest)
			>> NVP(pets)
			>> NVP(trio)
			>> NVP(nicknames)
			>> NVP(bag)
			>> NVP(C)
		;
	}

  std::cout << "num_people = " << num_people << std::endl;
}
