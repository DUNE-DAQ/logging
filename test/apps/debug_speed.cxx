/**
 * @file debug_speed.cxx - demonstrate debug message speed when Issues are used.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *

Run:
                                           debug_speed
  or                                       debug_speed 1  # this
  or                DUNEDAQ_ERS_DEBUG_LEVEL=2 debug_speed
  or                DUNEDAQ_ERS_DEBUG_LEVEL=2 debug_speed 1
  or  TRACE_LVLS=-1                        debug_speed
  or  TRACE_LVLS=-1                        debug_speed 1
  or  TRACE_LVLS=-1 DUNEDAQ_ERS_DEBUG_LEVEL=2 debug_speed    # or this
 */

#include <logging/Logging.hpp>
#include <string>

ERS_DECLARE_ISSUE(ers,		// namespace
                  File2,		// issue class name
                  "simple message with file="<<file_name, // no message
                  ((const char *)file_name ) // single attribute
                  )


int main(int argc, char *argv[] __attribute__((__unused__)))
{
	// activate TRACE memory buffer for debugging
	std::string tfile="/tmp/trace_buffer_"+std::string(getenv("USER"))+"_debug_speed";
	system( ("rm -f "+tfile).c_str() );
	setenv("TRACE_FILE",tfile.c_str(),0);

	static const int dbglvl=1;
	setenv("TRACE_LVLM",std::to_string(1ULL<<(TLVL_DEBUG+dbglvl)).c_str(),0); // make sure the specific debug lvl used is enabled

	if (argc > 1) {
	  dunedaq::logging::Logging::setup("test", "application");	// either do this or export DUNEDAQ_ERS_FATAL=erstrace,lstderr DUNEDAQ_ERS_ERROR='erstrace,throttle(30,100),lstderr' DUNEDAQ_ERS_WARNING='erstrace,throttle(30,100),lstderr'
	}

	ers::File2 efile2(ERS_HERE, "constructed file" );

	for (int ii=0; ii<10; ++ii)
		TLOG_DEBUG(dbglvl) << ers::File2( ERS_HERE, "construct file" );

	for (int ii=0; ii<10; ++ii)
		TLOG_DEBUG(dbglvl) << efile2;

	for (int ii=0; ii<10; ++ii)
		TLOG_DEBUG(dbglvl) << "message file does not exist";
	
	TLOG() << "\ntshow follows:\n\n";
	system( "TRACE_SHOW=\"%H%x%N %T %P %i %C %e %L %R %m\" trace_cntl show | trace_delta -ct 1 -d 1" );

	return (0);
}   // main
