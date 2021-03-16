/**
 * @file six_streams.cxx - another basic demonstration - can experiment with setup vs. env.var config
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
 */
const char *usage = R"foo(
  usage: %s [option]    # demonstrate six logging levels
example: %s
options:
 --help, -h       - print this help
 --no-setup, -n   - do not call Logging.setup() which integrates ERS and TRACE
)foo""\n";
#define USAGE() printf( usage, basename(argv[0]), basename(argv[0]))

#include <getopt.h>             // getopt_long
#include <libgen.h>             // basename

#include <logging/Logging.hpp>
#include <ers/SampleIssues.hpp>
#include <string>

int main(int argc, char *argv[])
{
	int opt_help=0, do_setup=1;
	while (true) {
		static struct option long_options[] = {
			{ "help",     no_argument,  nullptr,   'h' },
			{ "no-setup", no_argument,  nullptr,   'n' },
			{  nullptr,   0,            nullptr,    0  }
		};
		int opt = getopt_long( argc, argv, "?hn",long_options, nullptr );
		if (opt == -1) break;
		switch (opt) {
		case '?': case 'h': opt_help=1;     break;
		case 'n':           do_setup=0;     break;
		default:
			dunedaq::logging::Logging::setup();
			TLOG() << "?? getopt returned character code 0" << std::oct << opt;
			opt_help=1;
		}
	}
	if (opt_help) { USAGE(); exit(0); }

	if (do_setup) {
		dunedaq::logging::Logging::setup();	// either do this or export DUNEDAQ_ERS_FATAL=erstrace,lstderr DUNEDAQ_ERS_ERROR='erstrace,throttle(30,100),lstderr' DUNEDAQ_ERS_WARNING='erstrace,throttle(30,100),lstderr'
	}

	// usually, one of these (group of 3) do not come first
	ers::fatal(  ers::FileDoesNotExist( ERS_HERE, "fatal file" ) );
	ers::error(  ers::FileDoesNotExist( ERS_HERE, "error file" ) );
	ers::warning(ers::FileDoesNotExist( ERS_HERE, "warning file") );
	ers::info(   ers::FileDoesNotExist( ERS_HERE, "info file") );
	TLOG()   << "Hello log stream";
    TLOG_DEBUG(200)<<"debug 200-first debug (to ers::debug stream) appears to always get lvl 0.";
	TLOG_DEBUG(2) << "debug 2 - first debug (to ers::debug stream) appears to always get lvl 0.";
	TLOG_DEBUG(1) << "debug 1 - first debug (to ers::debug stream) appears to always get lvl 0.";
	TLOG_DEBUG(0) << "debug 0 - stream arg=" << 3;

	return (0);
}   // main
