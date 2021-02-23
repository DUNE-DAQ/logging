/**
 * @file debug_speed.cxx - demonstrate debug message speed when Issues are used.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *

Run:
                                           debug_speed --no-setup
  or                                       debug_speed             # this
  or                TDAQ_ERS_DEBUG_LEVEL=2 debug_speed --no-setup
  or                TDAQ_ERS_DEBUG_LEVEL=2 debug_speed
  or  TRACE_LVLS=-1                        debug_speed --no-setup
  or  TRACE_LVLS=-1                        debug_speed
  or  TRACE_LVLS=-1 TDAQ_ERS_DEBUG_LEVEL=2 debug_speed  --no-setup # or this
 */

const char *usage = R"foo(
  usage: %s [option]    # debug speed
example: %s
options:
 --help, -h          - print this help
 --loops, -l <loops> - loops each thread
 --mask, -m <mask>   - select b0=
 --no-setup          - do not do Logging().setup()
)foo""\n";
#define USAGE() printf( usage, basename(argv[0]), basename(argv[0]) )

#include <getopt.h>             // getopt_long
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

	int mask = 0x7;
	int opt_help=0;
	int loops=10;
	bool do_setup=true;
	while (true) {
		static struct option long_options[] = {
			{ "help",     no_argument,       nullptr,   'h' },
			{ "loops",    required_argument, nullptr,   'l' },
			{ "mask",     required_argument, nullptr,   'm' },
			{ "no-setup", no_argument,       nullptr,   'n' },
			{  nullptr,   0,                 nullptr,    0  }
		};
		int opt = getopt_long( argc, argv, "?hl:m:n",long_options, nullptr );
		if (opt == -1) break;
		switch (opt) {
		case '?': case 'h': opt_help   =1;                                          break;
		case 'l':           loops      =static_cast<int>(strtoul(optarg,nullptr,0));break;
		case 'm':           mask       =static_cast<int>(strtoul(optarg,nullptr,0));break;
		case 'n':           do_setup   =false;                                      break;
		default:
			TLOG() << "?? getopt returned character code 0" << std::oct << opt;
			opt_help=1;
		}
	}
	if (opt_help) { USAGE(); exit(0); }
	if (do_setup)
		Logging().setup();	// either do this or export TDAQ_ERS_FATAL=erstrace,lstderr TDAQ_ERS_ERROR='erstrace,throttle(30,100),lstderr' TDAQ_ERS_WARNING='erstrace,throttle(30,100),lstderr'

	ers::File2 efile2(ERS_HERE, "constructed file" );

	if (mask&1)
		for (int ii=0; ii<loops; ++ii)
			TLOG_DEBUG(dbglvl) << ers::File2( ERS_HERE, "construct file" );

	if (mask&2)
		for (int ii=0; ii<loops; ++ii)
			TLOG_DEBUG(dbglvl) << efile2;

	if (mask&4)
		for (int ii=0; ii<loops; ++ii)
			TLOG_DEBUG(dbglvl) << "message file does not exist";
	
	if (mask&8)
		for (int ii=0; ii<loops; ++ii)
			TLOG() << efile2;
	
	TLOG() << "\ntshow follows:\n\n";
	system( "TRACE_SHOW=\"%H%x%N %T %P %i %C %e %L %R %m\" trace_cntl show | trace_delta -ct 1 -d 1" );

	return (0);
}   // main
