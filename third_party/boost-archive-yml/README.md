# Boost-YML-archive

*Alfredo Correa 2019-2020*

Boost.YML-archive, a YAML-format archive compatible with Boost.Serialization. (This is not an official Boost library.)

Focus is on complete serialization/deserialization, allowing human-readability and limited manual modification when possible.

## Usage

The library is header only, although it might need linking to Boost.Serialization binary files.
(A hack to avoid linking to Boost.Serialization is provided by defining a macro `_MAKE_BOOST_SERIALIZATION_HEADER_ONLY`)

YAML archives are used just like any other archive in Boost.Serialization, for example XML archives.

## Serialization Example

code:
```c++
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
{
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
```

compilation and execution:
```shell
c++ -std=c++14 example.cpp -o example -lboost_serialization; ./example
```

(`-lboost_serialization` can be replaced by `-D_MAKE_BOOST_SERIALIZATION_HEADER_ONLY` to avoid linking.)

The sample YAML output is:

```yaml
---
# YAML 1.2 # boost/archive
salute: "Hello, World!"
num_people: 12
square:
  first: 4.20000000000000000e+01
  second: 8.40000000000000000e+01
tallest:
  initialized: true
  value: 1.22999999999999998e+00
pets:
  count: 4
  item0: "Bob"
  item1: "Rex"
  item2: "Tom"
  item3: "Jerry"
trio:
  elems:
    count: 3
    item0: 1.10000000000000009e+00
    item1: 2.20000000000000018e+00
    item2: 3.29999999999999982e+00
nicknames:
  count: 5
  item0:
    count: 4
    item0: "Aaron"
    item1: "Erin"
    item2: "Ronnie"
    item3: "Ron"
  item1:
    count: 3
    item0: "Adam"
    item1: "Edie"
    item2: "Ade"
  item2:
    count: 3
    item0: "Albert"
    item1: "Bert"
    item2: "Al"
  item3:
    count: 4
    item0: "Alfred"
    item1: "Freddy"
    item2: "Al"
    item3: "Fred"
  item4:
    count: 4
    item0: "Wilfred"
    item1: "Will"
    item2: "Willie"
    item3: "Fred"
bag:
  count: 3
  item0: 1.00000000000000000e+00
  item1: 2.00000000000000000e+00
  item2: 3.00000000000000000e+00
C:
  x: 1.10000000000000009e+00
  y: -2
  z: 3
...
# vim: set sw=2 ts=2 sts=2 tw=80 et nowrap
```

## Deserialization example:

Later on the variables can be reconstructed from the YAML file:

```c++
{
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
}
```

## Usage

In general, saving and loading an incoherent number of variables in separate code is not the best use for serialization archives.
The power of the library consists in keeping the serialization and deserialization code close together, ideally the same.
For example a typical small class can be (de)serialized in a single function.
The operator `&` is used to replace the two operators `>>` and `<<`.

```c++
struct custom{
	double x; int y; unsigned z;
	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar & NVP(x) & NVP(y) & NVP(z);
	}
};
```

Additionally, the serialization code of simpler classes can be reutilized.

```c++
#include <boost/serialization/vector.hpp>

struct custom2{
	std::vector<double> a;
	custom c;
	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar & NVP(a) & NVP(c);
	}
};
```

In particular, common types can be automatically serialized by code included in the Boost.Serialization library (headers).
For example, `boost/serialization/vector.hpp` in the case above.

Finally, the serialization code is written once and can be used with other archives types, such as XML, binary and text.
The documentation for the Boost.Serialization library can be found here https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/index.html

# What is serialization anyway?

While YAML files are human readable, it is important to note that the serialization philosophy doesn't provide a flexible human modification except for very simple cases.
Most modifications of the text file can render the file unusable.
For example manually replacing an integer into a decimal number in an archive file will not make the variable floating point when read, but possibly produce bad behavior.
A lot of information about the source and destination types is contained in the program itself and not in the file.
The program is for the most part still statically, not dynamically, typed.

Since manual or automatic modification intermediate between writes and reads is not part of the goals of serialization, the code doesn't check for the most part for well formed input.
Manual modification requires at least a deep understanding of the serialization code.

Some manual edits are probably ok, like changing a quoted string, changing decimal numbers, changing an integer for another (changing sign can problematic for unsigned types in the program), or adding/removing elements from a collection (dynamic container) (the element count needs to change as well!)


