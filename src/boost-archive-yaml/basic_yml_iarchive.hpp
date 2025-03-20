#ifndef BOOST_ARCHIVE_BASIC_YML_IARCHIVE_HPP
#define BOOST_ARCHIVE_BASIC_YML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_xml_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/type_index.hpp>

#include <boost/config.hpp>
#include <boost/mpl/assert.hpp>


#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

#include<optional>

#include<iostream>

#include "yml_archive_exception.hpp"
#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost {
namespace archive {

namespace detail {
    template<class Archive> class interface_iarchive;
} // namespace detail

/////////////////////////////////////////////////////////////////////////
// class basic_xml_iarchive - read serialized objects from a input text stream
template<class Archive>
class BOOST_SYMBOL_VISIBLE basic_yml_iarchive :
    public detail::common_iarchive<Archive>
{
    unsigned int depth;
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    friend class detail::interface_iarchive<Archive>;
#endif
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_start(const char *name)//;
	{
		assert(name);
	//	if(std::string(name) == "item"){
	//		char c = 0; while(c!= '-' and c!= ':'){
	//			this->This()->get_is().get(c);
	//			std::cout << 'c' << c << std::flush;
	//		}
	//		this->This()->get_is().get(c);
	//		assert( this->This()->get_is() );
	//	}else{
		std::string name_got;
			char c = 0; 
			while((c==' ') or (c=='\n')) this->This()->get_is().get(c);
			while(c!= ':'){
				this->This()->get_is().get(c);
				if(c!='\n' and c!=' ') name_got += c;
			}
			if((std::string{name} != "item") and name_got != (std::string{name} + ":")){
				std::cout<<"got name '"<< name_got <<"'"<<std::flush;
				std::cout<<"... expected was '"<< (std::string{name} + ":")<<"'";
				std::cout<<std::endl;
			//	assert(0);
			}
			assert( this->This()->get_is() );
		//	bool result = this->This()->gimpl->parse_start_tag(this->This()->get_is());
		//	std::cerr<<__FILE__<<": line "<<__LINE__<<": "<<__PRETTY_FUNCTION__<<std::endl;
		//	if(true != result){
		//		boost::serialization::throw_exception(
		//		    archive_exception(archive_exception::input_stream_error)
		//		);
		//	}
	//	}
	//	std::cerr<<__FILE__<<": line "<<__LINE__<<": "<<__PRETTY_FUNCTION__<<std::endl;
		// don't check start tag at highest level
		++depth;
		return;
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_end(const char *name)//;
	{
	//	std::cerr<<"looking for end tag '" << name <<"'"<<std::endl;
		// if there's no name
		assert(name);
	//	if(std::string(name) == "item"){
		//	char c = 0; while(c!='-') this->This()->get_is().get(c);
		//	this->This()->get_is().get(c);
	//	}else{
		//	bool result = this->This()->gimpl->parse_end_tag(this->This()->get_is());
		/*	if(true != result){
				boost::serialization::throw_exception(
				    archive_exception(archive_exception::input_stream_error)
				);
			}*/
			// don't check start tag at highest level
			if(0 == --depth)
				return;
				
		//	if(0 == (this->get_flags() & no_xml_tag_checking)){
		//		// double check that the tag matches what is expected - useful for debug
		//		if(0 != name[this->This()->gimpl->rv.object_name.size()]
		//		|| ! std::equal(
		//		        this->This()->gimpl->rv.object_name.begin(),
		//		        this->This()->gimpl->rv.object_name.end(),
		//		        name
		//		    )
		//		){
		//		    boost::serialization::throw_exception(
		//		        yml_archive_exception(
		//		            yml_archive_exception::yml_archive_tag_mismatch,
		//		            name
		//		        )
		//		    );
		//		}
		//	}
	//	}
	}

    // Anything not an attribute and not a name-value pair is an
    // should be trapped here.
    template<class T>
    void load_override(T & t)
    {
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        BOOST_MPL_ASSERT((serialization::is_wrapper< T >));
        this->detail_common_iarchive::load_override(t);
    }
    
    void load_override(bool&){
		assert(0);
    }

    typedef detail::common_iarchive<Archive> detail_common_iarchive;
    template<class T>
    void load_override(
		boost::serialization::nvp<T> const& t
    ) try{
//		std::cerr<<__FILE__<<": line "<<__LINE__<<": "<<__PRETTY_FUNCTION__<<" NAME="<<t.name()<< std::endl;
        this->This()->load_start(t.name());
//		std::cerr<<__FILE__<<": line "<<__LINE__<<": "<<__PRETTY_FUNCTION__<<std::endl;
        this->detail_common_iarchive::load_override(t.value());
//		std::cerr<<__FILE__<<": line "<<__LINE__<<": "<<__PRETTY_FUNCTION__<<std::endl;
        this->This()->load_end(t.name());
    }catch(std::runtime_error& e){
		std::string line;
		getline(this->This()->get_is(), line);
		throw std::runtime_error{"Couldn't load_override T = "+boost::typeindex::type_id<T>().pretty_name()+" because "+ e.what() + " line is "+ line};
	}catch(boost::archive::archive_exception& e){
		throw std::runtime_error{"ccouldn't load_override T = "+boost::typeindex::type_id<T>().pretty_name()+" because "+ e.what()};
	}
	
	void load_override(
		::boost::serialization::nvp<bool> const& t){
		this->This()->load_start(t.name());
	//	this->detail_common_iarchive::load_override(t.value());
		std::string bul; this->This()->get_is() >> bul;
		     if(bul == "true" ) t.value() = true;
		else if(bul == "false") t.value() = false;
		else{std::cout<<"expected bool, got "<< bul <<std::endl; assert(0);}
		this->This()->load_end(t.name());		
    }

#if __cpp_lib_optional
    template<class T>
    void load_override(
        const ::boost::serialization::nvp<std::optional<T>>& t
    ){
        this->This()->load_start(t.name());
	//	std::string s; this->This()->get_is() >> s;
	//	std::cerr<< s <<std::endl;
	//	assert(0);
	//	return;
		char c = '\0'; 
		while(c!='[') this->This()->get_is().get(c);
		if(this->This()->get_is().peek()!=']'){
			t.value() = T{};
			this->detail_common_iarchive::load_override(*t.value());
		}
		while(c!=']') this->This()->get_is().get(c);
    //    this->detail_common_oarchive::save_override(t.const_value());
        this->This()->load_end(t.name());

    }
#endif
/*    template<class T, std::size_t N>
    void load_override(
        const ::boost::serialization::nvp<T[N]>& t
    ){
		for(std::size_t n = 0; n != N; ++n){
			this->This()->load_start("item");
			this->detail::common_iarchive<Archive>::load_override(t.value()[n]);
			this->This()->load_end("item");
		}
	//    this->This()->save_end(t.name());

     //   this->This()->save_start(t.name());
	//	if(t.const_value().is_initialized()) this->detail_common_oarchive::save_override(*t.const_value());
	//	else this->This()->put("[ ]");
    //    this->detail_common_oarchive::save_override(t.const_value());
    //    this->This()->save_end(t.name());
    }
*/
    // specific overrides for attributes - handle as
    // primitives. These are not name-value pairs
    // so they have to be intercepted here and passed on to load.
    // although the class_id is included in the xml text file in order
    // to make the file self describing, it isn't used when loading
    // an xml archive.  So we can skip it here.  Note: we MUST override
    // it otherwise it will be loaded as a normal primitive w/o tag and
    // leaving the archive in an undetermined state
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(class_id_type & t)//;
	{
		t = class_id_type(this->This()->gimpl->rv.class_id);
	}
    void load_override(class_id_optional_type & /* t */){}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(object_id_type & t)//;
	{
		t = object_id_type(this->This()->gimpl->rv.object_id);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(version_type & t)//;
	{
		t = version_type(this->This()->gimpl->rv.version);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_override(tracking_type & t)//;
	{
		t = this->This()->gimpl->rv.tracking_level;
	}
    // class_name_type can't be handled here as it depends upon the
    // char type used by the stream.  So require the derived implementation
    // handle this.
    // void load_override(class_name_type & t);

    BOOST_ARCHIVE_OR_WARCHIVE_DECL
    basic_yml_iarchive(unsigned int flags)//;
	:
		detail::common_iarchive<Archive>(flags),
		depth(0)
	{}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL
    ~basic_yml_iarchive()//;
	{}
};

} // namespace archive
} // namespace boost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_ARCHIVE_BASIC_XML_IARCHIVE_HPP
