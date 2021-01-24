/**
 * @file performance.cxx - test the performance of the logger interface
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include <logging/Logger.hpp>

int main(/*int	argc, char	*argv[]*/)
{
	TLOG_DEBUG(0) << "hello from DEBUG_0";
	TLOG_DEBUG(6) << "hello from DEBUG_6";
	return (0);
}   // main
