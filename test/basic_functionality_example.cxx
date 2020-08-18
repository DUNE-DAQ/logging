/**
 * @file logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
/*
  cd build/logging
  make clean basic_functionality_example CXX_DEFINES=-DTRY_COMPILE=0
  test/basic_functionality_example
  or
  TDAQ_ERS_VERBOSITY_LEVEL=2 test/basic_functionality_example
  or
  TRACE_LVLS=-1 test/basic_functionality_example
 */

#define TRACE_NAME "basic_functionality_example" // next version (after v3_15_09) will do this automatically
#include <logging/Logger.hpp>
#include <ers/Issue.h>

/** \def ers::File2 This is the base class for all file related issues.
*/
ERS_DECLARE_ISSUE(ers,		// namespace
					  File2,		// issue class name
					  ERS_EMPTY, // no message
					  ((const char *)file_name ) // single attribute
                      )
/** \def ers::CantOpenFile2 This issue is reported when a certain file can
* not be opened by any reason.
*/
ERS_DECLARE_ISSUE_BASE(ers,	// namespace
                           CantOpenFile2, // issue class name
                           ers::File2,	 // base class name
                           "Can not open \"" << file_name << "\" file", // message
                           ((const char *)file_name ), // base class attributes
                           ERS_EMPTY				   // no attributes in this class
                           )

ERS_DECLARE_ISSUE(appframework, // namespace
                      CommandNotRegistered2, // issue class name
                      "Command '" << command_name
                      << "' does not have an entry in the CommandOrderMap!"
                      << " UserModules will receive this command in an unspecified order!",
                      ((const char *)command_name ) // single attribute
                      )

ERS_DECLARE_ISSUE(appframework, // namespace
                      MyExit, // issue class name
                  "exiting",
                  ERS_EMPTY				   // no attributes in this class
                           )

void ex_thread( volatile const int *spinlock, int thread_idx )
{
	while(*spinlock);			// The main program thread will clear this
								// once all thread are created and given a
								// chance to get here.
	for (auto uu=0; uu<5; ++uu)
		LOG_DEBUG(8) << "tidx " << thread_idx << " fast "<<uu;
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
	LOG_INFO()  << ers::Message(ERS_HERE,"A LOG_INFO()   using ers::Message - The Logger has just been setup.");
	LOG_LOG()   << ers::Message(ERS_HERE,"A LOG_LOG()    using ers::Message - The Logger has just been setup.");
	LOG_DEBUG(6)<< ers::Message(ERS_HERE,"A LOG_DEBUG(6) using ers::Message - The Logger has just been setup. ERS bug - 1st DEBUG is always DEBUG_0");
	LOG_DEBUG(6)<< ers::CantOpenFile2(ERS_HERE,"My_Fatal_FileName_via_LOG_DEBUG_6");
	LOG_LOG()   << ers::CantOpenFile2(ERS_HERE,"My_Fatal_FileName_via_LOG_LOG");
	LOG_INFO()  << ers::CantOpenFile2(ERS_HERE,"My_Fatal_FileName_via_LOG_INFO");
	LOG_INFO() << "The Logger has just been setup and this is an Info log message (LOG_INFO).";

	//----------------------------------------

	LOG_FATAL() << ers::CantOpenFile2(ERS_HERE,"My_Fatal_FileName - usually assiated with throw or exit");
	//ers::error( ers::Message( ERS_HERE, "this is ers::error( ers::Message( ERS_HERE, \"this is ...\" ) )" ) );
	LOG_ERROR() << ers::CantOpenFile2(ERS_HERE,"My_Error_FileName");
#   if TRY_COMPILE & 0x1
	// see what happens with: make clean install CXX_DEFINES=-DTRY_COMPILE=1
	LOG_ERROR() << "error with just a string";
#   endif
	LOG_WARNING() << ers::CantOpenFile2(ERS_HERE,"My_Warn_FileName");
	LOG_WARNING(ignore) << ers::Message(ERS_HERE,"My_Warn_Message with ignored macro param");

	//----------------------------------------

	LOG_INFO("TEST1") << ers::Message(ERS_HERE,"a specific TraceStreamer method with ers::Message isn't defined.");
	LOG_LOG("TEST1")  << appframework::CommandNotRegistered2(ERS_HERE,"MyCommand");

	LOG_DEBUG(lvl_t::d00) << "hello - debug level 0 mapped to 5";
	LOG_DEBUG(lvl_t::d05) << "hello - debug level 5";
	LOG_DEBUG(lvl_t::d06) << "hello - debug level 6";
	LOG_DEBUG(7) << ers::CantOpenFile2(ERS_HERE,"My_d07_FileName");

	LOG_DEBUG(lvl_t::d08,"TEST2") << "testing name argument";
	LOG_DEBUG(lvl_t::d63) << "debug lvl 63";

	std::cout << "\ntshow follows:\n\n";
	system( "TRACE_SHOW=\"%H%x%N %T %P %i %C %e %l %R %m\" trace_cntl show | trace_delta -ct 1 -d 1" );

	std::cout << "\nOne could try the same with TDAQ_ERS_VERBOSITY_LEVEL=2 or 3\n";

	std::cout << "\nNow, fast multithread...\n";
	const int num_threads=5;
	std::thread threads[num_threads];
	int spinlock=1;
	for (int uu=0; uu<num_threads; ++uu)
		threads[uu] = std::thread(ex_thread,&spinlock,uu);

	usleep(20000);
	spinlock = 0;

	for (unsigned uu=0; uu<num_threads; ++uu)
		threads[uu].join();

	std::cout << "\ntshow follows:\n\n";
	system( "TRACE_SHOW=\"%H%x%N %T %P %i %C %e %L %R %m\" trace_cntl show -c 25 | trace_delta -ct 1 -d 1" );

	throw( appframework::MyExit(ERS_HERE) );
	return (0);
}   // main
