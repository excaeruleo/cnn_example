#ifndef BOOST_ARCHIVE_BASIC_YML_ARCHIVE_EXCEPTION_HPP
#define BOOST_ARCHIVE_BASIC_YML_ARCHIVE_EXCEPTION_HPP

namespace boost {
namespace archive {

//////////////////////////////////////////////////////////////////////
// exceptions thrown by xml archives
//
class BOOST_SYMBOL_VISIBLE yml_archive_exception : 
    public virtual boost::archive::archive_exception
{
public:
    typedef enum {
        yml_archive_parsing_error,    // see save_register
        yml_archive_tag_mismatch,
        yml_archive_tag_name_error
    } exception_code;
    BOOST_ARCHIVE_DECL yml_archive_exception(
        exception_code c, 
        const char * e1 = NULL,
        const char * /*e2*/ = NULL
    )//;
    {
        switch(c){
        case yml_archive_parsing_error:
            archive_exception::append(0, "unrecognized XML syntax");
            break;
        case yml_archive_tag_mismatch:{
            unsigned int l;
            l = archive_exception::append(0, "XML start/end tag mismatch");
            if(NULL != e1){
                l = archive_exception::append(l, " - ");
                archive_exception::append(l, e1);
            }    
            break;
        }
        case yml_archive_tag_name_error:
            archive_exception::append(0, "Invalid XML tag name");
            break;
        default:
            BOOST_ASSERT(false);
            archive_exception::append(0, "programming error");
            break;
        }
    }
    BOOST_ARCHIVE_DECL yml_archive_exception(yml_archive_exception const & oth)// ;
	: archive_exception(oth)
	{
	}
    virtual BOOST_ARCHIVE_DECL ~yml_archive_exception() BOOST_NOEXCEPT_OR_NOTHROW// ;
	{}
};

}// namespace archive
}// namespace boost'

#endif

