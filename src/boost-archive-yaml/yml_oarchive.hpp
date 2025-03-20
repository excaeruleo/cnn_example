#if COMPILATION
$CXXX $CXXFLAGS -g $0 -o $0x -lboost_serialization -D_MAKE_BOOST_SERIALIZATION_HEADER_ONLY&&$0x&&rm $0x;exit
#endif
// © Alfredo Corres 2019-2020

#ifndef BOOST_ARCHIVE_YML_OARCHIVE_HPP
#define BOOST_ARCHIVE_YML_OARCHIVE_HPP

//#include <ostream> //for debug
#include<iomanip>
#include <cstddef> // size_t
//#include <boost/config.hpp>
//#if defined(BOOST_NO_STDC_NAMESPACE)
//namespace std{ 
//    using ::size_t; 
//} // namespace std
//#endif

#include <boost/archive/iterators/xml_escape.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/mb_from_wchar.hpp>

#include <boost/archive/detail/auto_link_archive.hpp>
#include <boost/archive/basic_text_oprimitive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <boost/serialization/item_version_type.hpp>

#if _MAKE_BOOST_SERIALIZATION_HEADER_ONLY
// use this to avoid need for linking -lserialization
//#include <boost/archive/detail/decl.hpp>
#if BOOST_VERSION > 106000 && BOOST_VERSION < 106600
#include "impl/singleton.cpp"
#endif
#if BOOST_VERSION < 105900
#define BOOST_ARCHIVE_DECL
#define BOOST_SERIALIZATION_DECL
#endif
#include "impl/archive_exception.cpp"
#include "impl/extended_type_info.cpp"
#include "impl/extended_type_info_typeid.cpp"
#include "impl/basic_archive.cpp"
//#include "impl_hack/basic_iarchive.cpp"
#include "impl/basic_oarchive.cpp"
//#include "impl_hack/basic_iserializer.cpp"
#include "impl/basic_oserializer.cpp"
#include "impl/basic_text_oprimitive.cpp"
#endif
//#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

//#include "../archive/basic_yml_oarchive.hpp"
#include <boost/config.hpp>
#include <boost/mpl/assert.hpp>


#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

//#include <boost/archive/detail/abi_prefix.hpp> // must be the last header
#include <exception>
#include <boost/assert.hpp>

#include <boost/config.hpp> 
#include <boost/archive/detail/decl.hpp>
#include <boost/archive/archive_exception.hpp>

#include <boost/type_index.hpp>

//#include<boost/units/quantity.hpp>
#include<boost/units/io.hpp>

#include<boost/optional.hpp>
#include<optional>
#include<iostream>
#include<map>
//#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

#include "yml_archive_exception.hpp"

namespace boost {
namespace archive {

class yml_null_t{};

}}

//#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

#include "basic_yml_archive.hpp"
namespace boost {
namespace archive {

namespace detail {
    template<class Archive> class interface_oarchive;
} // namespace detail

namespace detail {
template<class CharType>
struct YML_name {
    void operator()(CharType t) const{
        const unsigned char lookup_table[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0, // -.
            1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0, // 0-9
            0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // A-
            1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, // -Z _
            0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // a-
            1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0, // -z
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        };
        if((unsigned)t > 127)
            return;
        if(0 == lookup_table[(unsigned)t])
            boost::serialization::throw_exception(
                yml_archive_exception(
                    yml_archive_exception::yml_archive_tag_name_error
                )
            );
    }
};

} // namespace detail

//////////////////////////////////////////////////////////////////////
// class basic_xml_oarchive - write serialized objects to a xml output stream
template<class Archive>
class basic_yml_oarchive :
    public detail::common_oarchive<Archive>
{
private:
    // special stuff for yml output
    unsigned int depth = 0;
    bool pending_preamble;
    bool is_items = false;
    std::vector<int> items_v;
protected:
    friend class detail::interface_oarchive<Archive>;
    bool indent_next = false;

    void indent(){for(int i = depth; i-- > 0;) this->This()->put("  ");}
	void init(){
		this->This()->put("---\n");
		this->This()->put("# YAML 1.2 # boost/archive");
	}
	void windup(){
		this->This()->put("\n...");
		this->This()->put("\n# vim: set sw=2 ts=2 sts=2 tw=80 et nowrap");
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    write_attribute(
        const char *attribute_name,
        int t,
        const char *conjunction = ":"//"=\""
    )//;
	{
	//	this->This()->put(' ');
		this->This()->put(attribute_name);
		this->This()->put(conjunction);
		this->This()->put("179 ");
		this->This()->save(t);
	//	this->This()->put('"');
	}

    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    write_attribute(
        const char *attribute_name,
        const char *key
    )//;
	{
	//	this->This()->put(' ');
		this->This()->put(attribute_name);
		this->This()->put("=\"");
		this->This()->save(key);
		this->This()->put('"');
	}
	inline static void validate(char const* name){
		std::for_each(name, name + std::strlen(name), detail::YML_name<const char>());
	}
    // helpers used below
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_start(const char *name)//;
	{
		assert(name);
		validate(name);
	    this->This()->put("\n");
	    
/*		if(depth > 0){
		    this->This()->put("205\n");
		    indent();
		}
*/
	//	if(std::string{name} != "item"){
		//	is_items = false;
			indent();
			++depth;
		if(name != std::string{"item"}){
			this->This()->save(name);
			this->This()->put(":");
		}else{
			items_v.resize(depth+1);
			std::string name2 = name + std::to_string(items_v[depth]);
			++items_v[depth];
			this->This()->save(name2.c_str());
			this->This()->put(":");
		}
	/*	}else{
			if(is_items == false){
				indent();
				this->This()->put("items:\n");
			}
			++depth;
			indent();
			++depth;
			this->This()->put("-");
		}*/
//		pending_space = true;
//		pending_preamble = true;
//		indent_next = false;
//		if(depth > 0){
//		//    this->This()->put('\n');
		//    indent();
//		}
	//	this->This()->put('<');
	//	auto const translation = [](std::string const& name){
	//		static const std::map<std::string, std::string> m = {{"item", "-"}};
	//		auto it = m.find(name);
	//		if(it != m.end()) return it->second;
	//		return std::string(name + ":");
	//	};
	//	this->This()->save(translation(name).c_str());
	}
	void save_end(const char * name){
		assert(name);
#if 0
		if(NULL == name) return;

		// be sure name has no invalid characters
		std::for_each(name, name + std::strlen(name), detail::YML_name<const char>());

		end_preamble();
#endif
	/*	if(std::string{name} == "item"){
			--depth;
			--depth;
			is_items = true;
		}else{*/
			--depth;
			items_v.resize(depth+2);
	//	}
#if 0
		if(indent_next){
		    this->This()->put("237\n"); // not used
		    indent();
		}else{
		//   this->This()->put("240 ");
		}
		indent_next = false;
	//	indent_next = true;
	//	this->This()->put("</");
	//	this->This()->save(name);
	//	this->This()->put('>');
		if(0 == depth) this->This()->put("247\n");
#endif
	}

    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    end_preamble()//;
	{
		if(pending_preamble){
		//	if(indent_next) this->This()->put("_");

		//    this->This()->put(": ");
//		    this->This()->put('>');
		    pending_preamble = false;
		}
	}
    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template<class T>
    void save_override(T & t)
    {
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        BOOST_MPL_ASSERT((serialization::is_wrapper< T >));
        this->detail_common_oarchive::save_override(t);
    }
    
    template<class T>
    void save_override(bool const&)
    {
    	assert(0);
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
   //     BOOST_MPL_ASSERT((serialization::is_wrapper< T >));
    //    this->detail_common_oarchive::save_override(t);
    }

    // special treatment for name-value pairs.
    typedef detail::common_oarchive<Archive> detail_common_oarchive;
    template<class T, std::enable_if_t<not std::is_fundamental<T>{}, int> =0>
    void save_override(const ::boost::serialization::nvp<T>& t){
//		std::cout << boost::typeindex::type_id<const ::boost::serialization::nvp< T > &>().pretty_name() << std::endl;
        this->This()->save_start(t.name());
#if 0
		if(not indent_next) this->This()->put("179 ");
#endif
        this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
    template<class T, std::enable_if_t<std::is_fundamental<T>{}, int> =0>
    void save_override(::boost::serialization::nvp<T> const& t){
//		std::cout << boost::typeindex::type_id<const ::boost::serialization::nvp< T > &>().pretty_name() << std::endl;
		static_assert( not std::is_same<T, bool>{}, "!" );

        this->This()->save_start(t.name());
#if 0
		if(not indent_next) this->This()->put("179 ");
#endif
		this->This()->put(" ");
        this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }

#if 0
    template<class T>
    void save_override(
        const ::boost::serialization::nvp<std::optional<T>>& t
    ){
    //    this->This()->save_start(t.name());
    //    this->detail_common_oarchive::save_override(t.const_value());
    //    this->This()->save_end(t.name());
        this->This()->save_start(t.name());
		this->This()->put(" ");
		if(t.const_value()){
		//	this->This()->put("[");
		//	T arr[1] = {*t.const_value()};
			this->detail_common_oarchive::save_override(*t.value());
			this->This()->put(" # ~");
		}
		else this->This()->put("~");
    //    this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
    template<class T>
    void save_override(
        const ::boost::serialization::nvp<boost::optional<T>>& t
    ){
    //    this->This()->save_start(t.name());
    //    this->detail_common_oarchive::save_override(t.const_value());
    //    this->This()->save_end(t.name());
        this->This()->save_start(t.name());
		this->This()->put(" ");
		if(t.const_value().is_initialized()){
		//	this->This()->put("[");
		//	T arr[1] = {*t.const_value()};
			this->detail_common_oarchive::save_override(*t.value());
			this->This()->put(" # ~");
		}
		else this->This()->put("~");
    //    this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
#endif
    void save_override(bool const&){
		assert(0);
	//	this->This()->save_start(t.name());
	//	this->This()->put(t.value()?" true":" false");
	//	this->This()->save_end(t.name());
	}

	void save_override(::boost::serialization::nvp<bool> const& t){
		this->This()->save_start(t.name());
		this->This()->put(t.value()?" true":" false");
		this->This()->save_end(t.name());
	}

	void save_override(::boost::serialization::nvp<bool const> const& t){
		this->This()->save_start(t.name());
		this->This()->put(t.value()?" true":" false");
		this->This()->save_end(t.name());
	}

#if 0
    template<class U, class Y>
    void save_override(
		const ::boost::serialization::nvp<boost::units::quantity<U, Y>>& t
	){
		this->This()->save_start(t.name());
		this->detail_common_oarchive::save_override(t.value().value());
	//	this->This()->put(" # unit:");
		this->This()->put(" # ");
		std::ostringstream oss; oss<<"*"<< U{};
		this->This()->put(oss.str().c_str());
		this->This()->save_end(t.name());
	}
#endif
#if 0
    template<class U, class... Ts>
    void save_override(
		const ::boost::serialization::nvp<boost::units::quantity<U, Ts...>>& t
    ){
    //    this->This()->save_start(t.name());
    //    this->detail_common_oarchive::save_override(t.const_value());
    //    this->This()->save_end(t.name());
    //    this->This()->save_start(t.name());
		this->detail_common_oarchive::save_override(*t.const_value());
		this->indent();
		this->This()->put("\n#");
     //   this->This()->save_end(t.name());
    }
#endif
#if 0
    template<class T, std::size_t N>
    void save_override(
        const ::boost::serialization::nvp<T[N]>& t
    ){
    //    this->This()->save_start(t.name());
    //    this->detail_common_oarchive::save_override(t.const_value());
    //    this->This()->save_end(t.name());
//		assert(0);
 	//   this->This()->save_start(t.name());
		for(std::size_t n = 0; n != N; ++n){
	 	   this->This()->save_start("item");
    	    this->detail_common_oarchive::save_override(t.const_value()[n]);
	 	   this->This()->save_end("item");
	 	//   this->This()->save_end("item");		
		}
	//    this->This()->save_end(t.name());

     //   this->This()->save_start(t.name());
	//	if(t.const_value().is_initialized()) this->detail_common_oarchive::save_override(*t.const_value());
	//	else this->This()->put("[ ]");
    //    this->detail_common_oarchive::save_override(t.const_value());
    //    this->This()->save_end(t.name());
    }
#endif
#if 0
    template<class T>
    void save_override(
        const ::boost::serialization::nvp<std::optional<T> const>& t
    ){
        this->This()->save_start(t.name());
		if(t.const_value().has_value()){
			this->This()->put("[");
			this->detail_common_oarchive::save_override(*t.const_value());
			this->This()->put("]");
		}
		else this->This()->put("[]");
    //    this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
    template<class T>
    void save_override(
        const ::boost::serialization::nvp<std::optional<T>>& t
    ){
        this->This()->save_start(t.name());
		if(t.const_value().has_value()){
			this->This()->put("[");
			this->detail_common_oarchive::save_override(*t.const_value());
			this->This()->put("]");
		}
		else this->This()->put("[]");
    //    this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
#endif
    void save_override(
        const ::boost::serialization::nvp<unsigned>& t
    ){
        this->This()->save_start(t.name());
		this->This()->put(" ");
	//	this->This()->put("+");
		this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
    void save_override(
        const ::boost::serialization::nvp<unsigned long>& t
    ){
        this->This()->save_start(t.name());
		this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
    template<class T>
    void save_override(
        const ::boost::serialization::nvp<unsigned long long>& t
    ){
        this->This()->save_start(t.name());
		this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }
    // specific overrides for attributes - not name value pairs so we
    // want to trap them before the above "fall through"
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_id_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_ID(), t);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_id_optional_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_ID(), t);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_id_reference_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_ID_REFERENCE(), t);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const object_id_type &)// t)//;
	{
		// borland doesn't do conversion of STRONG_TYPEDEFs very well
//		const unsigned int i = t;
//		write_attribute(BOOST_ARCHIVE_YML_OBJECT_ID(), i, "=\"_");
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const object_reference_type &)// t)//;
	{
//		int i = t;//.t; // extra .t is for borland
//		write_attribute(BOOST_ARCHIVE_YML_OBJECT_REFERENCE(), i, "=\"_");
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const version_type &)// t)//;
	{
//		int i = t;//.t; // extra .t is for borland
//		write_attribute(BOOST_ARCHIVE_YML_VERSION(), i);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_name_type &)// t)//;
	{
//		const char * key = t;
//		if(NULL == key)
//		    return;
//		write_attribute(BOOST_ARCHIVE_YML_CLASS_NAME(), key);
	}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const tracking_type &)// t)//;
	{
//		write_attribute(BOOST_ARCHIVE_YML_TRACKING(), t.t);
	}
    void 
    save_override(const ::boost::serialization::nvp<boost::serialization::item_version_type const> &){
//        save(static_cast<unsigned int>(t));
    }
    void save_override(
        const ::boost::serialization::nvp<boost::serialization::collection_size_type>& t
    ){
//		assert(0);
        this->This()->save_start(t.name());
        this->This()->put(" ");
		this->detail_common_oarchive::save_override(t.const_value());
       this->This()->save_end(t.name());
	//	this->This()->put("\n"); this->indent();
	//	this->This()->put("items:");
    }
    void 
    save(const boost::serialization::item_version_type &){// t){
  //      save(static_cast<unsigned int>(t));
    }
    BOOST_ARCHIVE_OR_WARCHIVE_DECL
    basic_yml_oarchive(unsigned int flags)//;
	:
		detail::common_oarchive<Archive>(flags),
		depth(0),
		pending_preamble(false)
	{}
    BOOST_ARCHIVE_OR_WARCHIVE_DECL
    ~basic_yml_oarchive(){}
};

} // namespace archive
} // namespace boost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

//#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas


//#include <boost/core/uncaught_exceptions.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost {
namespace archive {

namespace detail {
    template<class Archive> class interface_oarchive;
} // namespace detail

// wide char stuff used by char archives
#ifndef BOOST_NO_CWCHAR
// copy chars to output escaping to xml and translating wide chars to mb chars
template<class InputIterator>
void save_iterator(std::ostream &os, InputIterator begin, InputIterator end){
    typedef boost::archive::iterators::mb_from_wchar<
        boost::archive::iterators::xml_escape<InputIterator>
    > translator;
    std::copy(
        translator(begin),
        translator(end),
        boost::archive::iterators::ostream_iterator<char>(os)
    );
}

//#ifndef BOOST_NO_STD_WSTRING
//template<class Archive>
//BOOST_ARCHIVE_DECL void
//xml_oarchive_impl<Archive>::save(const std::wstring & ws){
////  at least one library doesn't typedef value_type for strings
////  so rather than using string directly make a pointer iterator out of it
////    save_iterator(os, ws.data(), ws.data() + std::wcslen(ws.data()));
//    save_iterator(os, ws.data(), ws.data() + ws.size());
//}
//#endif

//#ifndef BOOST_NO_INTRINSIC_WCHAR_T
//template<class Archive>
//BOOST_ARCHIVE_DECL void
//xml_oarchive_impl<Archive>::save(const wchar_t * ws){
//    save_iterator(os, ws, ws + std::wcslen(ws));
//}
//#endif
#endif // BOOST_NO_CWCHAR

template<class Archive>
class BOOST_SYMBOL_VISIBLE yml_oarchive_impl : 
    public basic_text_oprimitive<std::ostream>,
    public basic_yml_oarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    friend class detail::interface_oarchive<Archive>;
    friend class basic_yml_oarchive<Archive>;
    friend class save_access;
#endif
    template<class T>
    void save(const T & t){
        basic_text_oprimitive<std::ostream>::save(t);
    }
    void 
    save(const version_type & t){
        save(static_cast<unsigned int>(t));
    }
    void 
    save(const boost::serialization::item_version_type & t){
        save(static_cast<unsigned int>(t));
    }
    BOOST_ARCHIVE_DECL void 
    save(const char * t)//;
	{
		typedef boost::archive::iterators::xml_escape<
		    const char * 
		> xml_escape_translator;
		std::copy(
		    xml_escape_translator(t),
		    xml_escape_translator(t + std::strlen(t)),
		    boost::archive::iterators::ostream_iterator<char>(os)
		);
	}
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    BOOST_ARCHIVE_DECL void
    save(const wchar_t * t)//;
	{
		save_iterator(os, t, t + std::wcslen(t));
	}
    #endif
    BOOST_ARCHIVE_DECL void
    save(const std::string &s)//;
	{
	//  at least one library doesn't typedef value_type for strings
	//  so rather than using string directly make a pointer iterator out of it
		os<<' '<< std::quoted(s);
	//	os << '\"';
	//	typedef boost::archive::iterators::xml_escape<
	//	    const char * 
	//	> xml_escape_translator;
	//	std::copy(
	//	    xml_escape_translator(s.data()),
	//	    xml_escape_translator(s.data()+ s.size()),
	//	    boost::archive::iterators::ostream_iterator<char>(os)
	//	);
	//	os << '\"';
	}
//	template<class T>
//	void save(boost::optional<T> const& s){
//		if(s) save(*s); else os << "~";
//	}
    #ifndef BOOST_NO_STD_WSTRING
    BOOST_ARCHIVE_DECL void
    save(const std::wstring &ws);
    #endif
    BOOST_ARCHIVE_DECL 
    yml_oarchive_impl(std::ostream & os, unsigned int flags)//;
	: 
		basic_text_oprimitive<std::ostream>(
		    os,
		    0 != (flags & no_codecvt)
		),
		basic_yml_oarchive<Archive>(flags)
	{
		if(0 == (flags & no_header)){
		    this->init();
		}
	}
    BOOST_ARCHIVE_DECL 
    ~yml_oarchive_impl(){
		if(0 == (this->get_flags() & no_header))
		    this->windup();
	}
public:
    BOOST_ARCHIVE_DECL
    void save_binary(const void *address, std::size_t count);
};

} // namespace archive
} // namespace boost


#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

//#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas
#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost { 
namespace archive {

// we use the following because we can't use
// typedef xml_oarchive_impl<xml_oarchive_impl<...> > xml_oarchive;

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from xml_oarchive_impl instead.  This will
// preserve correct static polymorphism.
class BOOST_SYMBOL_VISIBLE yml_oarchive : 
    public yml_oarchive_impl<yml_oarchive>
{
public:
	template<class T> using nvp = ::boost::serialization::nvp<T>;
	template<class T>
	static ::boost::serialization::nvp<T> make_nvp(char const* c, T& t){return boost::serialization::make_nvp(c, t);}
	yml_oarchive(std::ostream & os, unsigned int flags = 0)
	: yml_oarchive_impl<yml_oarchive>(os, flags)
	{}
	~yml_oarchive(){}
};

} // namespace archive
} // namespace boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::yml_oarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif


#include <boost/config.hpp>

#include <boost/archive/detail/basic_iarchive.hpp>

#include <boost/optional.hpp>
#include <boost/move/utility_core.hpp>

#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>
#include <boost/serialization/detail/is_default_constructible.hpp>
#include <boost/serialization/force_include.hpp>

#include <boost/serialization/optional.hpp>

namespace boost { 
namespace serialization {

#if 0
template<class T>
void save(
    boost::archive::yml_oarchive& ar, 
    const boost::optional< T > & t, 
    const unsigned int /*version*/
){
//	assert(0);
    // It is an inherent limitation to the serialization of optional.hpp
    // that the underlying type must be either a pointer or must have a
    // default constructor.  It's possible that this could change sometime
    // in the future, but for now, one will have to work around it.  This can
    // be done by serialization the optional<T> as optional<T *>
#if 0
    #if ! defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS)
        BOOST_STATIC_ASSERT(
            boost::serialization::detail::is_default_constructible<T>::value
            || boost::is_pointer<T>::value
        );
    #endif
    const bool tflag = t.is_initialized();
    ar << boost::serialization::make_nvp("initialized", tflag);
    if (tflag){
        ar << boost::serialization::make_nvp("value", *t);
    }
#endif
	if(t.is_initialized()) ar << boost::serialization::make_nvp("", *t);
	else ar.This()->put("~");
}
#endif
#if 0
template<class T>
void save(
    boost::archive::yml_oarchive& ar, 
    const std::optional< T > & t, 
    const unsigned int /*version*/
){
	if(t.has_value()){
		ar.This()->put("[");
		ar.save_override(*t);
	//	ar << boost::serialization::make_nvp("", *t);
		ar.This()->put("]");
	}
	else ar.This()->put("[]");
}
#endif


#if 0
template<class T>
void load(
    yml_iarchive & ar, 
    boost::optional< T > & t, 
    const unsigned int version
){
#if 0
    bool tflag;
    ar >> boost::serialization::make_nvp("initialized", tflag);
    if(! tflag){
        t.reset();
        return;
    }

    if(0 == version){
        boost::serialization::item_version_type item_version(0);
        boost::archive::library_version_type library_version(
            ar.get_library_version()
        );
        if(boost::archive::library_version_type(3) < library_version){
            ar >> BOOST_SERIALIZATION_NVP(item_version);
        }
    }
    if(! t.is_initialized())
        t = T();
    ar >> boost::serialization::make_nvp("value", *t);
#endif
}
#endif

template<class T>
void serialize(
    boost::archive::yml_oarchive& ar, 
    boost::optional< T > & t, 
    const unsigned int version
){
    boost::serialization::split_free(ar, t, version);
}

#if __cpp_lib_optional
template<class T>
void serialize(
    boost::archive::yml_oarchive& ar, 
    std::optional< T > & t, 
    const unsigned int version
){
    boost::serialization::split_free(ar, t, version);
}
#endif

} // serialization
} // boost


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if not __INCLUDE_LEVEL__ // _TEST_BOOST_ARCHIVE_YML_OARCHIVE

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp> // serialize pair
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>

#include "yml_iarchive.hpp"

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>
//#include <boost/units/systems/si.>

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
};

using namespace boost::units;

int main(){
	std::string const salute = "Hello, World!";
	int const num_people = 12;
	std::pair<double, double> const surface = {42, 84};
	std::vector<std::string> const pets = {"Bobby", "Rex", "Tom", "Jerry"};
	std::vector<std::vector<std::string>> const names = {{"Bobby", "Bob", "Boba"}, {"Rex", "Rey", "Carlos"}, {"Tom", "Tin"}, {"Jerry", "Jeremias"}};
	std::set<double> const bag = {3, 2, 1};
	custom const C{1.1, -2, 3};
	custom2 C2{}; C2.x = 222; C2.a = "zzz";
	quantity<si::length> const distance = 1.*si::meter;
	bool const vegetarian = true;
	boost::optional<double> tallest = {1.23};
	boost::optional<double> no_tallest;
	std::array<double, 3> const trio = {1.1, 2.2, 3.3};
	{
		std::ofstream ofs{"custom.yml"};
		boost::archive::yml_oarchive yoa{ofs};//, boost::archive::no_header};
		yoa<< BOOST_SERIALIZATION_NVP(salute); 
		yoa<< BOOST_SERIALIZATION_NVP(num_people); 
		yoa<< BOOST_SERIALIZATION_NVP(surface);
		yoa<< BOOST_SERIALIZATION_NVP(pets);
		yoa<< BOOST_SERIALIZATION_NVP(names);
		yoa<< BOOST_SERIALIZATION_NVP(bag);
		yoa<< BOOST_SERIALIZATION_NVP(C);
		yoa<< BOOST_SERIALIZATION_NVP(C2);
		yoa<< BOOST_SERIALIZATION_NVP(distance);
		yoa<< BOOST_SERIALIZATION_NVP(vegetarian);
		yoa<< BOOST_SERIALIZATION_NVP(tallest);
		yoa<< BOOST_SERIALIZATION_NVP(no_tallest);
		yoa<< BOOST_SERIALIZATION_NVP(trio);
	}
	{
		std::ifstream ifs{"custom.yml"};
		boost::archive::yml_iarchive yia{ifs};//, boost::archive::no_header};
		std::string salute;
		yia>> BOOST_SERIALIZATION_NVP(salute);
		assert(salute == "Hello, World!");
	}
}
#endif
#endif

