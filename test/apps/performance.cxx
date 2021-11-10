/**
 * @file performance.cxx - test the performance of the logger interface
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

const char *usage = R"foo(
  usage: %s [option]    # demonstrate threading issues
example: %s
options:
 --help, -h       - print this help
 --loops, -l      - loops each thread
 --threads, -t    - threads in addition to main
 --do-issue, -i   - use issue
)foo""\n";
#define USAGE() printf( usage, basename(argv[0]), basename(argv[0]) )

#include <getopt.h>             // getopt_long
#include <vector>
#include <thread>
#include <logging/Logging.hpp>


ERS_DECLARE_ISSUE(ERS_EMPTY,                                           // namespace ERS_EMPTY ==> anonymous
                  TestIssue,                        // issue class name
                  "Three arg TestIssue - arg 1: " << arg1 << " arg 2: " << arg2 << " arg 3: " << arg3, 
                  ((size_t)arg1) ((int)arg2) ((int)arg3)
                  )


static int g_dbglvl=6;
static int g_loops=2;
static int g_do_issue=0;

void thread_func( volatile const int *spinlock, size_t thread_idx )
{
	int lcllvl = g_dbglvl+thread_idx+1;

	while(*spinlock);			// The main program thread will clear this
								// once all thread are created and given a
								// chance to get here.
	if (g_do_issue) {
		for (auto uu=0; uu<g_loops; ++uu)
			ers::debug( TestIssue( ERS_HERE, thread_idx, lcllvl, uu ), lcllvl );
	} else {
		for (auto uu=0; uu<g_loops; ++uu)
			TLOG_DEBUG(lcllvl) << "tidx " << thread_idx << " fast LOG_DEBUG(" << lcllvl << ") #" <<uu;
	}

    pthread_exit(nullptr);
}


int main(int argc, char *argv[])
{
	dunedaq::logging::Logging::setup();	// either do this or export DUNEDAQ_ERS_FATAL=erstrace,lstderr DUNEDAQ_ERS_ERROR='erstrace,throttle(30,100),lstderr' DUNEDAQ_ERS_WARNING='erstrace,throttle(30,100),lstderr'

	int num_threads = 2;
	int opt_help=0;
	while (true) {
		static struct option long_options[] = {
			{ "help",     no_argument,       nullptr,   'h' },
			{ "loops",    required_argument, nullptr,   'l' },
			{ "threads",  required_argument, nullptr,   't' },
			{ "do-issue", no_argument,       nullptr,   'i' },
			{  nullptr,   0,                 nullptr,    0  }
		};
		int opt = getopt_long( argc, argv, "?hil:t:x",long_options, nullptr );
		if (opt == -1) break;
		switch (opt) {
		case '?': case 'h': opt_help   =1;                                          break;
		case 'l':           g_loops    =static_cast<int>(strtoul(optarg,nullptr,0));break;
		case 't':           num_threads=static_cast<int>(strtoul(optarg,nullptr,0));break;
		case 'i':           g_do_issue=1;                                           break;
		default:
			TLOG() << "?? getopt returned character code 0" << std::oct << opt;
			opt_help=1;
		}
	}
	if (opt_help) { USAGE(); exit(0); }

	std::vector<std::thread> threads(num_threads);
	int spinlock=1;
	for (size_t ss=0; ss<threads.size(); ++ss)
		threads[ss] = std::thread(thread_func,&spinlock,ss);

	usleep(20000);
	spinlock = 0;
	if (g_do_issue) {
		for (int ii=0; ii<g_loops; ++ii)
			ers::info( TestIssue( ERS_HERE, 0, g_dbglvl, ii ) );
	} else {
		for (int ii=0; ii<g_loops; ++ii)
			TLOG_DEBUG(g_dbglvl) << "hello from DEBUG_" << g_dbglvl << " loop " << ii;
	}

	for (std::thread& tt : threads)
		tt.join();
	return (0);
}   // main
