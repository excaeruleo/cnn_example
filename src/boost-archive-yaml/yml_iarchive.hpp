#if COMPILATION_INSTRUCTIONS
$CXXX $CXXFLAGS -g -D_TEST_BOOST_ARCHIVE_YML_IARCHIVE $0 -o $0x -ldl -lboost_serialization -D_MAKE_BOOST_SERIALIZATION_HEADER_ONLY &&$0x&&rm $0x;exit
#endif
#ifndef BOOST_ARCHIVE_YML_IARCHIVE_HPP
#define BOOST_ARCHIVE_YML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

//#include <boost/stacktrace.hpp>

#if defined(__clang__) && defined(__CUDA__)
// clang compiling CUDA code, host mode.
//#define BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#endif

#include <istream>
//#include<iostream> // std::cout for debug
#include <boost/scoped_ptr.hpp>
#include <boost/archive/detail/auto_link_archive.hpp>
#include <boost/archive/basic_text_iprimitive.hpp>
#include "basic_yml_iarchive.hpp"
#include <boost/archive/detail/register_archive.hpp>
#include <boost/serialization/item_version_type.hpp>

#include <boost/serialization/array.hpp>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

#include "basic_yml_grammar.hpp"

namespace boost { 
namespace archive {

namespace detail {
    template<class Archive> class interface_iarchive;
} // namespace detail

//template<class CharType>
//class basic_yml_grammar;
typedef basic_yml_grammar<char> yml_grammar;

template<class Archive>
class yml_iarchive_impl : 
    public basic_text_iprimitive<std::istream>,
    public basic_yml_iarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    friend class detail::interface_iarchive<Archive>;
    friend class basic_yml_iarchive<Archive>;
    friend class load_access;
#endif
    // use boost:scoped_ptr to implement automatic deletion;
    boost::scoped_ptr<yml_grammar> gimpl;

    std::istream & get_is(){
        return is;
    }
    template<class T>
    void load(T & t){
        basic_text_iprimitive<std::istream>::load(t);
    }

    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const class_id_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_ID(), t);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const class_id_optional_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_ID(), t);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const class_id_reference_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_ID_REFERENCE(), t);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const object_id_type &)// t)//;
	{
		// borland doesn't do conversion of STRONG_TYPEDEFs very well
//		const unsigned int i = t;
//		write_attribute(BOOST_ARCHIVE_YML_OBJECT_ID(), i, "=\"_");
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const object_reference_type &)// t)//;
	{
//		int i = t;//.t; // extra .t is for borland
//		write_attribute(BOOST_ARCHIVE_YML_OBJECT_REFERENCE(), i, "=\"_");
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const version_type &)// t)//;
	{
//		int i = t;//.t; // extra .t is for borland
//		write_attribute(BOOST_ARCHIVE_YML_VERSION(), i);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const class_name_type &)// t)//;
	{
//		const char * key = t;
//		if(NULL == key)
//		    return;
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_NAME(), key);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(const tracking_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_TRACKING(), t.t);
	}
    void 
    load_override(const ::boost::serialization::nvp<boost::serialization::item_version_type>&){
//        save(static_cast<unsigned int>(t));
    }
    
    void load_override(
        const ::boost::serialization::nvp<boost::serialization::collection_size_type>& t
    ){
	//	assert(0);
        this->This()->load_start(t.name());
	//	this->detail_common_iarchive::load_override(t.value());
		this->detail::common_iarchive<Archive>::load_override(t.value());
		this->This()->load_end(t.name());
	//	char c = '\0'; while(c != ':') this->This()->get_is().get(c);
	//	this->This()->put("\n"); this->indent();
	//	this->This()->put("items:");
#if 0
        this->This()->save_start(t.name());
		this->detail_common_oarchive::save_override(t.const_value());
       this->This()->save_end(t.name());
		this->This()->put("\n"); this->indent();
		this->This()->put("items:");
#endif
    }
    void 
    load(version_type & /*t*/){
//        unsigned int v;
//        load(v);
//        t = version_type(v);
    }
    void 
    load(boost::serialization::item_version_type & /*t*/){
    //    unsigned int v;
    //    load(v);
    //    t = boost::serialization::item_version_type(v);
    }
    BOOST_ARCHIVE_DECL void
    load(char * t);
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    BOOST_ARCHIVE_DECL void
    load(wchar_t * t);
    #endif
    BOOST_ARCHIVE_DECL void
    load(std::string &s)//;
	{
	//	std::cerr<<__FILE__ <<": line "<<__LINE__<<": " <<__PRETTY_FUNCTION__<<std::endl;
	//	assert(0);
		is >> std::quoted(s);
	//	assert(0);
		assert(is);
	//	bool result = gimpl->parse_string(is, s);
	//	std::cerr<<__FILE__ <<": line "<<__LINE__<<": string "<<s <<__PRETTY_FUNCTION__<<std::endl;
	//	if(! result){
	//	    boost::serialization::throw_exception(
	//	        yml_archive_exception(yml_archive_exception::yml_archive_parsing_error)
	//	    );
	//	}
	}
    #ifndef BOOST_NO_STD_WSTRING
    BOOST_ARCHIVE_DECL void
    load(std::wstring &ws);
    #endif
    template<class T>
    void load_override(T & t){
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
        basic_yml_iarchive<Archive>::load_override(t);
	// some compilers need -DBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX to work on serialization of boost::optional
    }

    BOOST_ARCHIVE_DECL void
    load_override(class_name_type & t)//;
	{
		const std::string & s = gimpl->rv.class_name;
		if(s.size() > BOOST_SERIALIZATION_MAX_KEY_SIZE - 1)
		    boost::serialization::throw_exception(
		        archive_exception(archive_exception::invalid_class_name)
		   );
		char * tptr = t;
		std::memcpy(tptr, s.data(), s.size());
		tptr[s.size()] = '\0';
	}
    BOOST_ARCHIVE_DECL void
    init()//;
	{
	//	assert(0);
		gimpl->init(is);
	//	assert(0);
		this->set_library_version(
		    library_version_type(gimpl->rv.version)
		);
	}
    BOOST_ARCHIVE_DECL 
    yml_iarchive_impl(std::istream & is, unsigned int flags)//;
	 :
		basic_text_iprimitive<std::istream>(
		    is, 
		    0 != (flags & no_codecvt)
		),
		basic_yml_iarchive<Archive>(flags),
		gimpl(new yml_grammar())
	{
		if(0 == (flags & no_header))
		    init();
	}
    BOOST_ARCHIVE_DECL
    ~yml_iarchive_impl()//;
	{
	//	if(boost::core::uncaught_exceptions() > 0)
	//	    return;
		if(0 == (this->get_flags() & no_header)){
		    gimpl->windup(is);
		}
	}
};

} // namespace archive
} // namespace boost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas
#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost { 
namespace archive {

class BOOST_SYMBOL_VISIBLE yml_iarchive : 
    public yml_iarchive_impl<yml_iarchive>{
public:
	template<class T>
	using nvp = ::boost::serialization::nvp<T>;
	template<class T>
	static decltype(auto) make_nvp(char const* c, T&& t){return boost::serialization::make_nvp(c, std::forward<T>(t));}
	yml_iarchive(std::istream & is, unsigned int flags = 0) :
		yml_iarchive_impl<yml_iarchive>(is, flags)
	{}
    ~yml_iarchive(){
//		if(std::uncaught_exception())
//		    return;
		if(0 == (this->get_flags() & no_header)) gimpl->windup(is);
	}
};

} // namespace archive
} // namespace boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::yml_iarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef _TEST_BOOST_ARCHIVE_YML_IARCHIVE

#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>

#include<boost/serialization/optional.hpp>

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

#include<iostream>
#include<fstream>

struct custom{
	double x; int y; unsigned z;
	template<class Ar>
	void serialize(Ar& ar, unsigned){
		ar 
			& BOOST_SERIALIZATION_NVP(x) 
			& BOOST_SERIALIZATION_NVP(y) 
			& BOOST_SERIALIZATION_NVP(z)
		;
	}
	bool operator==(custom const& o) const{return std::tie(x,y,z)==std::tie(o.x,o.y,o.z);}
	bool operator!=(custom const& o) const{return not operator==(o);}
};

struct custom2 : custom{
	std::string a = "sss";
	template<class Ar>
	void serialize(Ar& ar, unsigned){
	//	boost::serialization::serialize(ar, boost::serialization::base_object<custom>(*this), 0);
		auto& base = boost::serialization::base_object<custom>(*this);
	//	auto& base = static_cast<custom&>(*this);
		boost::serialization::serialize(ar, base, 0);
		ar & BOOST_SERIALIZATION_NVP(base);
		//custom::serialize(ar, u);
		ar & BOOST_SERIALIZATION_NVP(a);
	}
	bool operator==(custom2 const& o) const{return a==o.a and custom::operator==(o);}
	bool operator!=(custom2 const& o) const{return not operator==(o);}
};

using namespace boost::units;

int main(){

	{
		std::ifstream ifs{"custom.yml"};
		boost::archive::yml_iarchive yia{ifs};//, boost::archive::no_header};

		std::string salute;// = "Hello, World!";
		yia>> BOOST_SERIALIZATION_NVP(salute);
		assert( salute == "Hello, World!" );
		
		int num_people;//= 12;
		yia>> BOOST_SERIALIZATION_NVP(num_people);
		assert( num_people == 12 );
		
		std::pair<double, double> surface;// = {42, 84};
		yia>> BOOST_SERIALIZATION_NVP(surface);
		assert(( surface == std::pair<double, double>{42, 84} ));


		std::vector<std::string> pets;// = {"Bobby", "Rex", "Tom", "Jerry"};
		yia>> BOOST_SERIALIZATION_NVP(pets);
		assert(( pets == std::vector<std::string>{"Bobby", "Rex", "Tom", "Jerry"} ));

		std::vector<std::vector<std::string>> names;// = {{"Bobby", "Bob", "Boba"}, {"Rex", "Rey", "Carlos"}, {"Tom", "Tin"}, {"Jerry", "Jeremias"}};	
		yia>> BOOST_SERIALIZATION_NVP(names);
		assert(( names == decltype(names){{"Bobby", "Bob", "Boba"}, {"Rex", "Rey", "Carlos"}, {"Tom", "Tin"}, {"Jerry", "Jeremias"}} ));
	
		std::set<double> bag;// = {3, 2, 1};
		yia>> BOOST_SERIALIZATION_NVP(bag);
		assert(( bag == decltype(bag){3, 2, 1} ));
		
		custom C;//{1.1, -2, 3};
		yia>> BOOST_SERIALIZATION_NVP(C);
		assert(( C == custom{1.1, -2, 3} ));

		custom2 C2; // C2.x = 222; C2.a = "zzz";
		yia>> BOOST_SERIALIZATION_NVP(C2);
		assert(( C2.x == 222 ));
		assert(( C2.a == "zzz" ));

		quantity<si::length> distance;// = 1.*si::meter;
		yia>> BOOST_SERIALIZATION_NVP(distance);
		assert( distance == 1.*si::meter );
		
		bool vegetarian = false;
		yia>> BOOST_SERIALIZATION_NVP(vegetarian);
		assert( vegetarian == true );
		
		boost::optional<double> tallest;// = {1.23};
		yia>> BOOST_SERIALIZATION_NVP(tallest);
		assert( tallest.is_initialized() );
		assert( *tallest == 1.23 );
		
		boost::optional<double> no_tallest;
		yia>> BOOST_SERIALIZATION_NVP(no_tallest);
		assert(not no_tallest.is_initialized() );
		
		std::array<double, 3> trio;// = {1.1, 2.2, 3.3};
		yia>> BOOST_SERIALIZATION_NVP(trio);
	//	std::array<double, 3> trio;// = {1.1, 2.2, 3.3};
	//	yia>> BOOST_SERIALIZATION_NVP(trio);
	//	assert(( trio == std::array<double, 3>{1.1, 2.2, 3.3} ));
	//	std::array<double, 3> trio;// = {1.1, 2.2, 3.3};
	//	yia>> BOOST_SERIALIZATION_NVP(trio);
	}

#if 0
	std::string salute;// = "Hello, World!";
	int num_people; // =12
	std::pair<double, double> surface;// = {42, 84};
	std::optional<double> tallest; // = 1.23;
	std::vector<std::string> pets;// = {"Bobby", "Rex", "Tom", "Jerry"};
	std::array<double, 3> trio;// = {1.1, 2.2, 3.3};
	std::vector<std::vector<std::string>> names;// = {{"Bobby", "Bob", "Boba"}, {"Rex", "Rey", "Carlos"}, {"Tom", "Tin"}, {"Jerry", "Jeremias"}};
	std::set<double> bag;// = {3, 2, 1};
	custom C{1.1, -2, 3};
	{
		std::ifstream ifs{"example.yml"};
		while(ifs){std::string s; getline(ifs, s); std::cout<< s <<std::endl;}
	}
	{
		std::ifstream ifs{"example.yml"};
		boost::archive::yml_iarchive yia{ifs, boost::archive::no_header};
		yia >> BOOST_SERIALIZATION_NVP(salute); std::cout << "salute-> " << salute << std::endl;
		yia >> BOOST_SERIALIZATION_NVP(num_people); std::cout << "num_people-> " << num_people << std::endl;
		yia >> BOOST_SERIALIZATION_NVP(surface); std::cout << "surface-> " << surface.first <<' '<< surface.second << std::endl;
		yia >> BOOST_SERIALIZATION_NVP(tallest); std::cout << "tallest-> "; if(tallest.has_value()) std::cout << *tallest << std::endl; else std::cout <<"~"<< std::endl;
		yia >> BOOST_SERIALIZATION_NVP(pets); std::cout << "pets size " << pets.size() << std::endl; std::cout << "pets[3] ->" << pets[3] << std::endl;
		yia >> BOOST_SERIALIZATION_NVP(trio); std::cout << "trio " << trio[2] << std::endl;
		yia >> BOOST_SERIALIZATION_NVP(names); std::cout << names[1][2] << std::endl;
		yia >> BOOST_SERIALIZATION_NVP(bag); std::cout << bag.size() << std::endl;
		yia >> BOOST_SERIALIZATION_NVP(C); std::cout << C.y << std::endl;
	}
#endif
}
#endif


#endif // BOOST_ARCHIVE_XML_IARCHIVE_HPP
