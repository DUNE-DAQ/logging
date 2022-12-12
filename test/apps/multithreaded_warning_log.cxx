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
#include <stdlib.h>			// rand_r, RAND_MAX
#include <vector>
#include <thread>
#define JUST_ERS 1
#if JUST_ERS
# include <ers/ers.hpp>
#else
# include <logging/Logging.hpp>
#endif


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

	unsigned seed=1;
	unsigned *seedp=&seed;

	while(*spinlock);			// The main program thread will clear this
								// once all thread are created and given a
								// chance to get here.
	for (auto uu=0; uu<g_loops; ++uu) {
		unsigned rr=rand_r(seedp);
		if (rr < RAND_MAX/2)
			ers::warning( TestIssue( ERS_HERE, thread_idx, lcllvl, uu ) );
		else if (g_do_issue)
# if JUST_ERS
			ers::log( TestIssue( ERS_HERE, thread_idx, lcllvl, uu ) );
# else
			TLOG() << TestIssue( ERS_HERE, thread_idx, lcllvl, uu );
		else
			TLOG() << "thread_idx=" << thread_idx << " uu=" << uu;
# endif
	}

    pthread_exit(nullptr);
}


int main(int argc, char *argv[])
{
# if JUST_ERS == 0
	dunedaq::logging::Logging::setup();	// either do this or export DUNEDAQ_ERS_FATAL=erstrace,lstderr DUNEDAQ_ERS_ERROR='erstrace,throttle(30,100),lstderr' DUNEDAQ_ERS_WARNING='erstrace,throttle(30,100),lstderr'
# endif

	int num_threads = 20;
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
# if JUST_ERS == 0
			TLOG() << "?? getopt returned character code 0" << std::oct << opt;
# endif
			opt_help=1;
		}
	}
	if (opt_help) { USAGE(); exit(0); }

	unsigned seed=1;
	unsigned *seedp=&seed;

	std::vector<std::thread> threads(num_threads);
	int spinlock=1;
	for (size_t ss=0; ss<threads.size(); ++ss)
		threads[ss] = std::thread(thread_func,&spinlock,ss);

	usleep(20000);
	spinlock = 0;
	for (auto uu=0; uu<g_loops; ++uu) {
		unsigned rr=rand_r(seedp);
		if (rr < RAND_MAX/2)
			ers::warning( TestIssue( ERS_HERE, 999, 998, uu ) );
		else if (g_do_issue)
# if JUST_ERS
			ers::log( TestIssue( ERS_HERE, 999, 998, uu ) );
# else
			TLOG() << TestIssue( ERS_HERE, 999, 998, uu );
		else
			TLOG() << " uu=" << uu;
# endif
	}

	for (std::thread& tt : threads)
		tt.join();
	return (0);
}   // main
