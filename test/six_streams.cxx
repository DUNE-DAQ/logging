/**
 * @file logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
 */

#include <logging/Logger.hpp>
#include <ers/SampleIssues.h>

int main()
{
	std::vector<std::string> arguments;
	Logger().setup(arguments);	// either do this or export TDAQ_ERS_FATAL=erstrace,lstderr TDAQ_ERS_ERROR='erstrace,throttle(30,100),lstderr' TDAQ_ERS_WARNING='erstrace,throttle(30,100),lstderr'

	LOG_FATAL()  << ers::FileDoesNotExist( ERS_HERE, "fatal file" );
	LOG_ERROR()  << ers::FileDoesNotExist( ERS_HERE, "error file" );
	LOG_WARNING()<< ers::FileDoesNotExist( ERS_HERE, "warning file" );
	LOG_INFO()  << "Hello info stream - Note: LOG_{FATAL,ERROR,WARNING,INFO} slow path always enabled";
	LOG_LOG()   << "Hello log stream";
	LOG_DEBUG(0)<< "Hello debug stream";

	return (0);
}   // main
