#ifndef BOOST_ARCHIVE_BASIC_XML_GRAMMAR_HPP
#define BOOST_ARCHIVE_BASIC_XML_GRAMMAR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_xml_grammar.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// this module is derived from simplexml.cpp - an example shipped as part of
// the spirit parser.  This example contains the following notice:
/*=============================================================================
    simplexml.cpp

    Spirit V1.3
    URL: http://spirit.sourceforge.net/

    Copyright (c) 2001, Daniel C. Nuffer

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the copyright holder be held liable for
    any damages arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute
    it freely, subject to the following restrictions:

    1.  The origin of this software must not be misrepresented; you must
        not claim that you wrote the original software. If you use this
        software in a product, an acknowledgment in the product documentation
        would be appreciated but is not required.

    2.  Altered source versions must be plainly marked as such, and must
        not be misrepresented as being the original software.

    3.  This notice may not be removed or altered from any source
        distribution.
=============================================================================*/
#include <string>
#include<iomanip>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/spirit/include/classic_rule.hpp>
#include <boost/spirit/include/classic_chset.hpp>

// spirit stuff
#include <boost/spirit/include/classic_operators.hpp>
#include <boost/spirit/include/classic_actions.hpp>
#include <boost/spirit/include/classic_numerics.hpp>

#include <boost/archive/basic_archive.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/version.hpp>

#include "yml_archive_exception.hpp"

#include "basic_yml_archive.hpp"

namespace boost {
namespace archive {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// XML grammar parsing

namespace yml { // anonymous

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

template<class T>
struct assign_impl {
    T & t;
    void operator()(const T t_) const {
        t = t_;
    }
    assign_impl(T &  t_)
        : t(t_)
    {}
};

template<>
struct assign_impl<std::string> {
    std::string & t;
    void operator()(
        std::string::const_iterator b, 
        std::string::const_iterator e
    ) const {
        t.resize(0);
        while(b != e){
            t += * b;
            ++b;
        }
    }
    assign_impl(assign_impl<std::string> const&) = default;
    assign_impl<std::string> & operator=(
        assign_impl<std::string> & rhs
    );
//    assign_impl<std::string> & operator=(assign_impl<std::string> const& rhs) = delete;
    assign_impl(std::string & t_)
        : t(t_)
    {}
};

#ifndef BOOST_NO_STD_WSTRING
template<>
struct assign_impl<std::wstring> {
    std::wstring & t;
    void operator()(
        std::wstring::const_iterator b, 
        std::wstring::const_iterator e
    ) const {
        t.resize(0);
        while(b != e){
            t += * b;
            ++b;
        }
    }
    assign_impl(std::wstring & t_)
        : t(t_)
    {}
};
#endif

template<class T>
assign_impl<T> assign_object(T &t){
    return assign_impl<T>(t);
} 

struct assign_level {
    tracking_type & tracking_level;
    void operator()(const unsigned int tracking_level_) const {
        tracking_level = (0 == tracking_level_) ? false : true;
    }
    assign_level(tracking_type &  tracking_level_)
        : tracking_level(tracking_level_)
    {}
};

template<class String, class Iterator>
struct append_string {
    String & contents;
    void operator()(Iterator start, Iterator end) const {
    #if 0
        typedef boost::archive::iterators::xml_unescape<Iterator> translator;
        contents.append(
            translator(BOOST_MAKE_PFTO_WRAPPER(start)), 
            translator(BOOST_MAKE_PFTO_WRAPPER(end))
        );
    #endif
        contents.append(start, end);
    }
    append_string(String & contents_)
        : contents(contents_)
    {}
};

template<class String>
struct append_char {
    String & contents;
    void operator()(const unsigned int char_value) const {
        contents += static_cast<typename String::value_type>(char_value);
    }
    append_char(String & contents_)
        : contents(contents_)
    {}
};

template<class String, unsigned int c>
struct append_lit {
    String & contents;
    template<class X, class Y>
    void operator()(const X & /*x*/, const Y & /*y*/) const {
        const typename String::value_type z = c;
        contents += z;
    }
    append_lit(String & contents_)
        : contents(contents_)
    {}
};

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

} // namespace anonymous

template<class CharType>
class BOOST_SYMBOL_VISIBLE basic_yml_grammar {
public:
    // The following is not necessary according to DR45, but at least
    // one compiler (Compaq C++ 6.5 in strict_ansi mode) chokes otherwise.
    struct return_values;
    friend struct return_values;
    
private:
    typedef typename std::basic_istream<CharType> IStream;
    typedef typename std::basic_string<CharType> StringType;
    typedef typename boost::spirit::classic::chset<CharType> chset_t;
    typedef typename boost::spirit::classic::chlit<CharType> chlit_t;
    typedef typename boost::spirit::classic::scanner<
        typename  std::basic_string<CharType>::iterator
    > scanner_t;
    typedef typename boost::spirit::classic::rule<scanner_t> rule_t;
    // Start grammar definition
    rule_t    
        Reference,
        Eq, 
        STag,
        ETag,
        LetterOrUnderscoreOrColon,
        AttValue, 
        CharRef1, 
        CharRef2, 
        CharRef, 
        AmpRef,
        LTRef,
        GTRef,
        AposRef,
        QuoteRef,
        CharData,
        CharDataChars,
        content,
        AmpName,
        LTName,
        GTName,
        ClassNameChar,
        ClassName,
        Name,
        XMLDecl,
        XMLDeclChars,
        DocTypeDecl,
        DocTypeDeclChars,
        ClassIDAttribute,
        ObjectIDAttribute,
        ClassNameAttribute,
        TrackingAttribute,
        VersionAttribute,
        UnusedAttribute,
        Attribute,
        SignatureAttribute,
        SerializationWrapper,
        NameHead,
        NameTail,
        AttributeList,
        S;

    // XML Character classes
    chset_t
        BaseChar,
        Ideographic,
        Char, 
        Letter, 
        Digit,
        CombiningChar,
        Extender, 
        Sch,
        NameChar;

    void init_chset()//;
	{
		Char = chset_t("\x9\xA\xD\x20-\x7f\x80\x81-\xFF"); 
		Letter = chset_t("\x41-\x5A\x61-\x7A\xC0-\xD6\xD8-\xF6\xF8-\xFF");
		Digit = chset_t("0-9");
		Extender = chset_t('\xB7');
		Sch = chset_t("\x20\x9\xD\xA");
//		NameChar = Letter | Digit | boost::spirit::classic::chset_p("._:-") | Extender ;
		NameChar = Letter | Digit | boost::spirit::classic::chset_p("._-") | Extender ;
	}
    bool my_parse(
        IStream & is,
        const rule_t &rule_, 
        const CharType delimiter = L' '
    ) const// ;
	{
		assert(0);
		if(is.fail()){
		    return false;
		}
		
		is >> std::noskipws;

		std::basic_string<CharType> arg;
		
		for(;;){
		    CharType result;
		    is.get(result);
		    if(is.fail()){
		        boost::serialization::throw_exception(
		            boost::archive::archive_exception(
		                archive_exception::input_stream_error,
		                std::strerror(errno)
		            )
		        );
		    }
		    if(is.eof())
		        return false;
		    arg += result;
		    if(result == delimiter or result =='\n')
		        break;
		}
		// read just one more character.  This will be the newline after the tag
		// this is so that the next operation will return fail if the archive
		// is terminated.  This will permit the archive to be used for debug
		// and transaction data logging in the standard way.
		
		boost::spirit::classic::parse_info<typename std::basic_string<CharType>::iterator> 
		    result = boost::spirit::classic::parse(arg.begin(), arg.end(), rule_);
		{
		    CharType result;
		    is.get(result);
		}
		return result.hit;
	}
public:
    struct return_values {
        StringType object_name;
        StringType contents;
        //class_id_type class_id;
        int_least16_t class_id;
        //object_id_type object_id;
        uint_least32_t object_id;
        //version_type version;
        unsigned int version;
        tracking_type tracking_level;
        StringType class_name;
        return_values() :
            version(0),
            tracking_level(false)
        {}
    } rv;
    bool parse_start_tag(IStream & is)// /*const*/;
	{
		while(is.get()==' '){if(not is) return false;}
		while(is.get()!=':'){if(not is) return false;}
//		std::cout << __PRETTY_FUNCTION__ << std::endl;
//		rv.class_name.resize(0);
//		auto ret = my_parse(is, STag);
//		assert(ret);
		return true;
	}
    bool parse_end_tag(IStream & is) const//;
	{
	//	CharType c = '\0'; while(c!='\n') is.get(c);
		assert(is);
		return true;
	//	return my_parse(is, ETag);
	}
    bool parse_string(IStream & is, StringType & s)// /*const*/;
	{
		assert(0);
		std::string out;
		is >> std::quoted(out);
		assert(is);
		s = out;
		return true;
	//	is >> s;
	//	return 0;
		rv.contents.resize(0);
		
		std::string arg;
		for(;;){
		    CharType result;
		    is.get(result);
		    if(is.fail()){
		        boost::serialization::throw_exception(
		            boost::archive::archive_exception(
		                archive_exception::input_stream_error,
		                std::strerror(errno)
		            )
		        );
		    }
		    if(is.eof())
		        return false;
		    arg += result;
		    if(result == '\n')
		        break;
		}
		s = arg;
		return true;
	//	std::cerr<<__FILE__ <<": line "<<__LINE__<<": s "<< s <<__PRETTY_FUNCTION__<<std::endl;
	//	return 0;
		
		bool result = my_parse(is, content, '\"');//<');
		// note: unget caused a problem with dinkumware.  replace with
	 // is.unget();
		// putback another delimiter instead
		is.putback('\"');//'<');
		if(result)
		    s = rv.contents;
		return result;
	}
    void init(IStream & is)//;
	{
		init_chset();
		std::string dashes; 
		getline(is, dashes); //assert(dashes=="---");
		std::string line;
		getline(is, line);
		if(line != "# YAML 1.2 # boost/archive"){
			std::cout<< line <<std::endl;
			assert(0);
		}
	//	assert(c == '\n');
	#if 0
		if(! my_parse(is, XMLDecl))
		    boost::serialization::throw_exception(
		        yml_archive_exception(yml_archive_exception::yml_archive_parsing_error)
		    );
		if(! my_parse(is, DocTypeDecl))
		    boost::serialization::throw_exception(
		        yml_archive_exception(yml_archive_exception::yml_archive_parsing_error)
		    );
		if(! my_parse(is, SerializationWrapper))
		    boost::serialization::throw_exception(
		        yml_archive_exception(yml_archive_exception::yml_archive_parsing_error)
		    );
		if(! std::equal(rv.class_name.begin(), rv.class_name.end(), BOOST_ARCHIVE_SIGNATURE()))
		    boost::serialization::throw_exception(
		        archive_exception(archive_exception::invalid_signature)
		    );
	#endif
	}
	bool windup(IStream&)
	{
	
	//	std::string line; 
	//	getline(is, line); std::cout <<"line "<< std::endl;
	//	getline(is, line); std::cout <<"line "<< std::endl;
	//	getline(is, line);
	//	if(line != "..."){
	//		std::cout<< "expected ... , got " << line << std::endl;
	//	}
	//	assert(0);
	//	return my_parse(is, ETag);
		return true;
	}
    basic_yml_grammar()//;
	{
		init_chset();

		S =
		    +(Sch)
		;

		// refactoring to workaround template depth on darwin
		NameHead = (Letter | '_');// | ':');
		NameTail = *NameChar ;
		Name =
		  NameHead >> NameTail
		;

		Eq =
		    !S >> '=' >> !S
		;

		AttributeList = 
		    *(S >> Attribute)
		;
		
		STag =
		   !S >>
		//    >> '<'
		//    >> 
			Name  [yml::assign_object(rv.object_name)]
		//    >> AttributeList
		    >> !S
		    >> ':'
		;

//		ETag =
//		    !S
//		    >> "</"
//		    >> Name [yml::assign_object(rv.object_name)]
//		    >> !S 
//		    >> '>'
//		;
		ETag =
		    !S
		    >> "\n"
		//    >> Name [yml::assign_object(rv.object_name)]
		 //   >> !S 
		  //  >> '>'
		;

		// refactoring to workaround template depth on darwin
		CharDataChars = +(boost::spirit::classic::anychar_p - boost::spirit::classic::chset_p(L"&<"));
		CharData =  
		    CharDataChars [
		        yml::append_string<
		            StringType, 
		            typename std::basic_string<CharType>::const_iterator
		        >(rv.contents)
		    ]
		;

		// slight factoring works around ICE in msvc 6.0
		CharRef1 = 
		    boost::spirit::classic::str_p(L"&#") >> boost::spirit::classic::uint_p [yml::append_char<StringType>(rv.contents)] >> L';'
		;
		CharRef2 =
		    boost::spirit::classic::str_p(L"&#x") >> boost::spirit::classic::hex_p [yml::append_char<StringType>(rv.contents)] >> L';'
		;
		CharRef = CharRef1 | CharRef2 ;

		AmpRef = boost::spirit::classic::str_p(L"&amp;")[yml::append_lit<StringType, L'&'>(rv.contents)];
		LTRef = boost::spirit::classic::str_p(L"&lt;")[yml::append_lit<StringType, L'<'>(rv.contents)];
		GTRef = boost::spirit::classic::str_p(L"&gt;")[yml::append_lit<StringType, L'>'>(rv.contents)];
		AposRef = boost::spirit::classic::str_p(L"&apos;")[yml::append_lit<StringType, L'\''>(rv.contents)];
		QuoteRef = boost::spirit::classic::str_p(L"&quot;")[yml::append_lit<StringType, L'"'>(rv.contents)];

		Reference =
		    AmpRef
		    | LTRef
		    | GTRef
		    | AposRef
		    | QuoteRef
		    | CharRef
		;

		content = 
		//    L"<" // should be end_p
		//    | 
			L"\"" >>
			+(Reference | CharData) >> L"\""//L"<"
		;

		ClassIDAttribute = 
		    boost::spirit::classic::str_p(BOOST_ARCHIVE_YML_CLASS_ID()) >> NameTail
		    >> Eq 
		    >> L'"'
		    >> boost::spirit::classic::int_p [yml::assign_object(rv.class_id)]
		    >> L'"'
		  ;

		ObjectIDAttribute = (
		    boost::spirit::classic::str_p(BOOST_ARCHIVE_YML_OBJECT_ID()) 
		    | 
		    boost::spirit::classic::str_p(BOOST_ARCHIVE_YML_OBJECT_REFERENCE()) 
		    )
		    >> NameTail
		    >> Eq 
		    >> L'"'
		    >> L'_'
		    >> boost::spirit::classic::uint_p [yml::assign_object(rv.object_id)]
		    >> L'"'
		;
		    
		AmpName = boost::spirit::classic::str_p(L"&amp;")[yml::append_lit<StringType, L'&'>(rv.class_name)];
		LTName = boost::spirit::classic::str_p(L"&lt;")[yml::append_lit<StringType, L'<'>(rv.class_name)];
		GTName = boost::spirit::classic::str_p(L"&gt;")[yml::append_lit<StringType, L'>'>(rv.class_name)];
		ClassNameChar = 
		    AmpName
		    | LTName
		    | GTName
		    | (boost::spirit::classic::anychar_p - boost::spirit::classic::chset_p(L"\"")) [yml::append_char<StringType>(rv.class_name)]
		;
		
		ClassName =
		    * ClassNameChar
		;
		
		ClassNameAttribute = 
		    boost::spirit::classic::str_p(BOOST_ARCHIVE_YML_CLASS_NAME()) 
		    >> Eq 
		    >> L'"'
		    >> ClassName
		    >> L'"'
		;

		TrackingAttribute = 
		    boost::spirit::classic::str_p(BOOST_ARCHIVE_YML_TRACKING())
		    >> Eq
		    >> L'"'
		    >> boost::spirit::classic::uint_p [yml::assign_level(rv.tracking_level)]
		    >> L'"'
		;

		VersionAttribute = 
		    boost::spirit::classic::str_p(BOOST_ARCHIVE_YML_VERSION())
		    >> Eq
		    >> L'"'
		    >> boost::spirit::classic::uint_p [yml::assign_object(rv.version)]
		    >> L'"'
		;

		UnusedAttribute = 
		    Name
		    >> Eq
		    >> L'"'
		    >> !CharData
		    >> L'"'
		;

		Attribute =
		    ClassIDAttribute
		    | ObjectIDAttribute
		    | ClassNameAttribute
		    | TrackingAttribute
		    | VersionAttribute
		    | UnusedAttribute
		;

		XMLDeclChars = *(boost::spirit::classic::anychar_p - boost::spirit::classic::chset_p(L"?>"));
		XMLDecl =
		    !S
		    >> boost::spirit::classic::str_p(L"<?xml")
		    >> S
		    >> boost::spirit::classic::str_p(L"version")
		    >> Eq
		    >> boost::spirit::classic::str_p(L"\"1.0\"")
		    >> XMLDeclChars
		    >> !S
		    >> boost::spirit::classic::str_p(L"?>")
		;

		DocTypeDeclChars = *(boost::spirit::classic::anychar_p - boost::spirit::classic::chset_p(L">"));
		DocTypeDecl =
		    !S
		    >> boost::spirit::classic::str_p(L"<!DOCTYPE")
		    >> DocTypeDeclChars
		    >> L'>'
		;

		SignatureAttribute = 
		    boost::spirit::classic::str_p(L"signature") 
		    >> Eq 
		    >> L'"'
		    >> Name [yml::assign_object(rv.class_name)]
		    >> L'"'
		;
		
		SerializationWrapper =
		    !S
		    >> boost::spirit::classic::str_p(L"<boost_serialization")
		    >> S
		    >> ( (SignatureAttribute >> S >> VersionAttribute)
		       | (VersionAttribute >> S >> SignatureAttribute)
		       )
		    >> !S
		    >> L'>'
		;
	}

};

} // namespace archive
} // namespace boost

#endif // BOOST_ARCHIVE_BASIC_XML_GRAMMAR_HPP
