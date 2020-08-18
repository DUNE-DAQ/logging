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
	LOG_DEBUG(lvl_t::d00) << "hello";
	return (0);
}   // main
