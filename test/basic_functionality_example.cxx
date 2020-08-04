#include <logging/logging.hpp>

/** \def ers::File This is the base class for all file related issues.
*/
ERS_DECLARE_ISSUE_HPP(ers,		// namespace
					  File,		// issue class name
					  ERS_EMPTY, // no message
					  ((const char *)file_name ) // single attribute
                      )
/** \def ers::CantOpenFile This issue is reported when a certain file can
* not be opened by any reason.
*/
ERS_DECLARE_ISSUE_BASE_HPP(ers,	// namespace
                           CantOpenFile, // issue class name
                           ers::File,	 // base class name
                           "Can not open \"" << file_name << "\" file", // message
                           ((const char *)file_name ), // base class attributes
                           ERS_EMPTY				   // no attributes in this class
                           )


int main(int argc, char *argv[])
{
	std::vector<std::string> arguments(argv + 1, argv + argc);
	setenv("TRACE_MSGMAX","0",0);
	setenv("TRACE_LVLS","-1",0);
	setenv("TRACE_LVLM","-1",0);
	Logger().setup(arguments);
	LOG_FATAL() << ers::CantOpenFile( ERS_HERE, "MyFatalFileName" );
	LOG_ERROR() << ers::CantOpenFile( ERS_HERE, "MyErrorFileName" );
	LOG_WARNING() << ers::CantOpenFile( ERS_HERE, "MyWarnFileName" );
#   if TRY_COMPILE & 0x1
	// see make happeins with: make clean install CXX_DEFINES=-DTRY_COMPILE=1
	LOG_ERROR() << "just a string";
#   endif
	//LOG_INFO()
	//LOG_LOG()
	LOG_DEBUG(0) << ers::CantOpenFile( ERS_HERE, "MyDebugFileName" );
	LOG_DEBUG(0) << "hello - debug level 0";
	LOG_DEBUG(1) << "hello - debug level 1";
	system( "trace_cntl show -c10 | trace_delta" );
	return (0);
}   // main
