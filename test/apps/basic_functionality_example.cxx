/**
 * @file logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
  cd build/logging
  make clean basic_functionality_example CXX_DEFINES=-DTRY_COMPILE=0

Run:
    install/logging/bin/basic_functionality_example
  or
    DUNEDAQ_ERS_VERBOSITY_LEVEL=2 install/logging/bin/basic_functionality_example
  or
    TRACE_LVLS=-1 install/logging/bin/basic_functionality_example
 */

#include <string>
#define TRACE_NAME "basic_functionality_example" // NOLINT next version (after v3_15_09) will do this automatically
#define ERS_PACKAGE "Logging"					 // Qualifier
#define DUNEDAQ_PACKAGE_NAME "Logging_"
#include <logging/Logging.hpp>
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
                       "Can not open \"" << file_name << "\" file arg2=" << arg2 << " arg3=" << arg3, // message
                       ((const char *)file_name ), // base class attributes
                       ((int) arg2) ((const char*) arg3)			   // two attributes in this class
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
		TLOG_DEBUG(8) << "tidx " << thread_idx << " fast LOG_DEBUG(8) #" <<uu;
}


int main(/*int argc, char *argv[]*/)
{
	// FOR THIS EXAMPLE ONLY -- NOT NORMALLY NEEDED!! --v--v--v--v--v--v
	// activate TRACE memory buffer for debugging
	std::string tfile="/tmp/trace_buffer_"+std::string(getenv("USER"))+"_basic";
	system( ("rm -f "+tfile).c_str() );
	setenv("TRACE_FILE",tfile.c_str(),0);
	setenv("TRACE_LVLM","-1",0);
	setenv("TRACE_LVLS","0xff",0);
	TRACE_CNTL("reset");
	// --^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--^--

	setenv("DUNEDAQ_APPLICATION_NAME","LOGGING_BASIC_FUN_APP",0);
	Logging().setup(); // not strictly needed -- checks/establishes a default env.

	TLOG_DEBUG( 0 )  << "a message which doesn't go to the central logger";
	TLOG() << "another example of a message that doesn't go to the central logger";
	TLOG("MYNAME") << "yet another example -- messages are controllable via name+level";

	ers::Message message(ERS_HERE,"Using TRACE_FILE="+tfile);
	message.add_qualifier( "Logging_qual2" );
	TLOG()  << message;

	//----------------------------------------

	ers::fatal( ers::CantOpenFile2(ERS_HERE,"My_Fatal_FileName - usually associated with throw or exit",4,"four") );
	//ers::error( ers::Message( ERS_HERE, "this is ers::error( ers::Message( ERS_HERE, \"this is ...\" ) )" ) );
	ers::error( ers::CantOpenFile2(ERS_HERE,"My_Error_FileName",5,"five") );
#   if TRY_COMPILE & 0x1
	// see what happens with: make clean install CXX_DEFINES=-DTRY_COMPILE=1
	ers::error( "error with just a string" );
#   endif
	ers::warning( ers::CantOpenFile2(ERS_HERE,"My_Warn_FileName",6,"six") );
	ers::warning( ers::Message(ERS_HERE,"My_Warn_Message with ignored macro param") );
	ers::info(    ers::Message(ERS_HERE,"a specific TraceStreamer method with ers::Message isn't defined.") );

	//----------------------------------------

	TLOG("TEST1")  << appframework::CommandNotRegistered2(ERS_HERE,"MyCommand");
	TLOG() << "LOG_LOG() stating LOG_DEBUG(n)'s follow -- they must be enabled via trace_cntl or DUNEDAQ_ERS_DEBUG_LEVEL";

	TLOG_DEBUG(6)<< ers::Message(ERS_HERE,"A LOG_DEBUG(6) using ers::Message - The Logging has just been setup. ERS bug - 1st DEBUG is always DEBUG_0");
	TLOG_DEBUG(6)<< ers::CantOpenFile2(ERS_HERE,"My_Fatal_FileName_via_LOG_DEBUG_6",7,"seven");
	TLOG_DEBUG(0) << "hello - debug level 0";
	TLOG_DEBUG(5) << "hello - debug level 5";
	TLOG_DEBUG(6) << "hello - debug level 6";
	TLOG_DEBUG(7) << ers::CantOpenFile2(ERS_HERE,"My_d07_FileName",8,"eight");

	TLOG_DEBUG(8,"TEST2") << "testing name argument";
	TLOG_DEBUG(55) << "debug lvl 55";
	TLOG_DEBUG(56) << "debug lvl 56";
	TLOG_DEBUG(63) << "debug lvl 63";
	TLOG_DEBUG(64) << "debug lvl 64";

	TLOG() << "\ntshow follows:\n\n";
	system( "TRACE_TIME_FMT=\"%Y-%b-%d %H:%M:%S,%%03d\" TRACE_SHOW=\"%H%x%N %T %e %l %L %m\" trace_cntl show | trace_delta -ct 1 -d 1" );

	TLOG() << "\nOne could try the same with DUNEDAQ_ERS_VERBOSITY_LEVEL=2 or 3\n";

	TLOG() << "\nNow, fast multithread...\n";
	const int kNumThreads=5;
	std::thread threads[kNumThreads];
	int spinlock=1;
	for (int uu=0; uu<kNumThreads; ++uu)
		threads[uu] = std::thread(ex_thread,&spinlock,uu);

	usleep(20000);
	spinlock = 0;

	for (unsigned uu=0; uu<kNumThreads; ++uu)
		threads[uu].join();

	TLOG() << "\ntshow follows:\n\n";
	system( "TRACE_SHOW=\"%H%x%N %T %P %i %C %e %L %R %m\" trace_cntl show -c 25 | trace_delta -ct 1 -d 1" );

	throw( appframework::MyExit(ERS_HERE) );
	return (0);
}   // main
