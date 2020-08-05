/*
  cd build/logging
  make clean basic_functionality_example CXX_DEFINES=-DTRY_COMPILE=0
  test/basic_functionality_example
 */

#define TRACE_NAME "basic_functionality_example" // next version (after v3_15_09) will do this automatically
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

void ex_thread( int *spinlock )
{
	while(*spinlock);
	for (unsigned uu=0; uu<5; ++uu)
		LOG_DEBUG(d08) << "fast "<<uu;
}


int main(int argc, char *argv[])
{
	std::vector<std::string> arguments(argv + 1, argv + argc);

	// activate TRACE memory buffer for debugging
	setenv("TRACE_MSGMAX","0",0);
	setenv("TRACE_LVLM","-1",0);
	setenv("TRACE_LVLS","0xff",0);
	TRACE_CNTL("reset");

	Logger().setup(arguments);
	LOG_INFO() << "The Logger has just been setup and this is the first Info log message (LOG_INFO).";

//ers::error( ers::Message( ERS_HERE, "this is ers::error( ers::Message( ERS_HERE, \"this is ...\" ) )" ) );

	LOG_FATAL() << ers::CantOpenFile(ERS_HERE,"My_Fatal_FileName");
	LOG_ERROR() << ers::CantOpenFile(ERS_HERE,"My_Error_FileName");
#   if TRY_COMPILE & 0x1
	// see what happens with: make clean install CXX_DEFINES=-DTRY_COMPILE=1
	LOG_ERROR() << "error with just a string";
#   endif
	LOG_WARNING() << ers::CantOpenFile(ERS_HERE,"My_Warn_FileName");
	LOG_WARNING(ignore) << ers::Message(ERS_HERE,"My_Warn_Message with ignored macro param");


	LOG_INFO() << "info uses ers::Message";
#   if TRY_COMPILE & 0x2
	// see what happens with: make clean install CXX_DEFINES=-DTRY_COMPILE=2
	LOG_INFO() << ers::Message(ERS_HERE,"a specific TraceStreamer method with ers::Message isn't defined.");
#   endif
	LOG_LOG()  << "LOG_LOG also uses ers::Message";
	LOG_LOG("TEST1")  << ers::CantOpenFile(ERS_HERE,"My_Log_FileName");

	LOG_DEBUG(d00) << "hello - debug level 0 mapped to 5";
	LOG_DEBUG(d05) << "hello - debug level 5";
	LOG_DEBUG(d06) << "hello - debug level 6";
	LOG_DEBUG(d07) << ers::CantOpenFile(ERS_HERE,"My_d07_FileName");

	LOG_DEBUG(d08,"TEST2") << "testing name argument";

	std::cout << "\ntshow follows:\n\n";
	system( "TRACE_SHOW=\"%H%x%N %T %P %i %C %n %l %R %m\" trace_cntl show | trace_delta -ct 1 -d 1" );

	std::cout << "\nOne could try the same with TDAQ_ERS_VERBOSITY_LEVEL=2 or 3\n";




	return (0);
}   // main
