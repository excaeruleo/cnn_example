#ifndef BOOST_ARCHIVE_BASIC_YML_ARCHIVE_HPP
#define BOOST_ARCHIVE_BASIC_YML_ARCHIVE_HPP

namespace boost{
namespace archive{

	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_OBJECT_ID(){
		return "@object_id";
	}
	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_OBJECT_REFERENCE(){
		return "@object_id_reference";
	}
	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_CLASS_ID(){
		return "@class_id";
	}
	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_CLASS_ID_REFERENCE(){
		return "@class_id_reference";
	}
	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_CLASS_NAME(){
		return "@class_name";
	}
	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_TRACKING(){
		return "@tracking_level";
	}
	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_VERSION(){
		return "@version";
	}
	inline BOOST_SYMBOL_VISIBLE const char *
	BOOST_ARCHIVE_YML_SIGNATURE(){
		return "@signature";
	}

}}

#endif

