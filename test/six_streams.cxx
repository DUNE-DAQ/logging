/**
 * @file six_streams.cxx - another basic demonstration - can experiment with setup vs. env.var config
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
 */

//#define TRACE_NAME "six_streams.cxx"
#include <logging/Logger.hpp>
#include <ers/SampleIssues.h>
#if TRACE_REVNUM < 1394
#	define TRACE_NAME trace_path_components(__FILE__,1)
#endif

int main(int argc, char *argv[])
{
	if (argc > 1) {
		std::vector<std::string> arguments(argv + 1, argv + argc);
		Logger().setup(arguments);	// either do this or export TDAQ_ERS_FATAL=erstrace,lstderr TDAQ_ERS_ERROR='erstrace,throttle(30,100),lstderr' TDAQ_ERS_WARNING='erstrace,throttle(30,100),lstderr'
	}

	// usually, one of these (group of 3) do not come first
	LOG_FATAL()  << ers::FileDoesNotExist( ERS_HERE, "fatal file" );
	LOG_ERROR()  << ers::FileDoesNotExist( ERS_HERE, "error file" );
	LOG_WARNING()<< ers::FileDoesNotExist( ERS_HERE, "warning file" );

	LOG_INFO()  << "Hello info stream - Note: LOG_{FATAL,ERROR,WARNING,INFO} slow path always enabled";
	LOG_LOG()   << "Hello log stream";
	LOG_DEBUG(0,1)<< "Hello debug%d stream arg=" << 3;
	LOG_DEBUG(1) << "Hello debug stream arg=" << 3;

	return (0);
}   // main
