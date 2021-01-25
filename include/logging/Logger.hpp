/**
 * @file Logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef LOGGING_INCLUDE_LOGGING_LOGGER_HPP_
#define LOGGING_INCLUDE_LOGGING_LOGGER_HPP_

#if !defined(ERS_PACKAGE) && defined(TRACE_NAME)
#	define ERS_PACKAGE TRACE_NAME
#elif !defined(TRACE_NAME) && defined(ERS_PACKAGE)
//#	define TRACE_NAME ERS_PACKAGE   // this may cause TRACE_NAME to be "unknown" (if ers/SampleIssues.h included before Logger.hpp)
#else
//  defaults will be used
#endif

#include <string>
#include <vector>
#include "ers/ers.h"
#include <ers/OutputStream.h>
#define TRACE_LOG_FUN_PROTO \
  static void erstrace_user(struct timeval *, int, uint8_t, const char*, const char*, int, const char*, uint16_t nargs, const char *msg, ...); \
  static void erstrace_user(struct timeval *, int, uint8_t, const char*, const char*, int, const char*, uint16_t nargs, const std::string& msg, ...)
#undef TRACE_LOG_FUNCTION
#define TRACE_LOG_FUNCTION erstrace_user
// NOTE: TRACE_*_LVLSTRS would only affect the application slow path, which
// in this case, is not applicable. Memory buffer formatted output (via
// trace_cntl) would use TRACE_*_LVLSTRS but is not being built.
#include "TRACE/trace.h"


//-----------------------------------------------------------------------------


/*  6 logging "streams" are defined: fatal, error, warning, info, log and debug.
    The first 4 are accessed via the ers:: methods with a TRACE destination added.
    THe last 2 are accessed via TRACE macros with an ers:: user method configured.
 */
// Redefine TRACE's TLOG to only take 2 optional args: name and format control
#undef TLOG
#define TLOG(...)  TRACE_STREAMER(TLVL_LOG, \
								  _tlog_ARG2(not_used, CHOOSE_(__VA_ARGS__)(__VA_ARGS__) 0,need_at_least_one), \
								  _tlog_ARG3(not_used, CHOOSE_(__VA_ARGS__)(__VA_ARGS__) 0,"",need_at_least_one), \
								  1, SL_FRC(TLVL_LOG) )






#include <logging/detail/Logger.hxx> // needs TLVL_* definitions


/**
 * @brief The Logger class defines the interface necessary to configure central
 * logging within a DAQ Application.
 */
class Logger
{
public:
  /**
   * @brief Setup the Logger service
   * @param args Command-line arguments used to setup the Logger
   */
	static void setup(const std::vector<std::string> & args __attribute__((__unused__)) = {})
	{
		// need to get tricky to short circuit DEBUG message (at "level 1") about
		//    libmtsStreams.so: cannot open shared object file: No such file or directory		
		/*  Example: production env:
			export TDAQ_ERS_ERROR="erstrace,throttle(30,100),lstderr,mts"
			export TDAQ_ERS_FATAL="erstrace,lstderr,mts"
			export TDAQ_ERS_WARNING="erstrace,throttle(30,100),lstderr,mts"
			export TDAQ_ERS_INFO="erstrace,throttle(30,100),lstdout,mts"

			export TDAQ_ERS_LOG="lstdout"
			export TDAQ_ERS_DEBUG="lstdout"

			export TDAQ_ERS_STREAM_LIBS="mtsStreams"
		 */
		setenv("TDAQ_ERS_FATAL", "erstrace,lstderr",0);
		setenv("TDAQ_ERS_ERROR", "erstrace,throttle(30,100),lstderr",0);
		setenv("TDAQ_ERS_WARNING","erstrace,throttle(30,100),lstderr",0);
		setenv("TDAQ_ERS_INFO",   "lstdout",0);
		setenv("TDAQ_ERS_LOG",    "lstdout",0);
		setenv("TDAQ_ERS_DEBUG",  "lstdout",0);

		//setenv("TDAQ_APPLICATION_NAME","XYZZY",0);
		std::ostringstream out;
		out << (1ULL<<(TLVL_DEBUG+2))-1;
		setenv("TRACE_LVLM",out.str().c_str(),0);

		// Avoid DEBUG_1 [ers::PluginManager::PluginManager(...) at ...Library mtsStreams can not be loaded because libmtsStreams.so: cannot open shared object file: No such file or directory
		// by only setting debug_level AFTER first ers::debug message
		//setenv("TDAQ_ERS_DEBUG_LEVEL","63",0);
		ers::LocalContext lc( "unknown", __FILE__, __LINE__, __func__, 0/*no_stack*/ );
		int lvl=1;
		ers::debug(ers::Message(lc,"hello") BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY(ERS_EMPTY lvl) ) ) lvl);
		ers::Configuration::instance().debug_level(63);
		char *cp;
		if ((cp=getenv("TDAQ_ERS_DEBUG_LEVEL")) && *cp) {
			int lvl=strtoul(cp,nullptr,0)+TLVL_DEBUG;
			if (lvl>63) lvl=63;
			//TRACE_CNTL("lvlmskSg",(1ULL<<lvl)-1); // this sets traceTID to id of "Logger"
			uint64_t msk = ((1ULL<<lvl)-1) | (1ULL<<lvl);
			std::string mskstr=std::to_string(msk);
			setenv("TRACE_LVLS",mskstr.c_str(),0);
		}
	}
};


// The following Stream macros a) accept strings or Issue objects, and b)
// accept optional arguments - NAME and/or "noDelayedFmt"
// The TRACE_STREAMER args are: 1-lvl, 2,3-nam_or_fmt, 4-slow_enabled, 5-slow_force
#undef  TLOG_INFO
#define TLOG_INFO(...) TRACE_STREAMER(TLVL_INFO, \
										  _tlog_ARG2(not_used, CHOOSE_(__VA_ARGS__)(__VA_ARGS__) 0,need_at_least_one), \
										  _tlog_ARG3(not_used, CHOOSE_(__VA_ARGS__)(__VA_ARGS__) 0,"",need_at_least_one), \
										  1, SL_FRC(TLVL_INFO) )

//  The following uses gnu extension of "##" connecting "," with empty __VA_ARGS__
//  which eats "," when __VA_ARGS__ is empty.
//      pragma GCC diagnostic ignored "-Wvariadic-macros" // doesn't seem to work.
//  Neither does pragma GCC diagnostic ignored "-Wpedantic" // g++ -std=c++11 --help=warnings | egrep -i 'variadic|pedantic'
//  These are last because "pragma GCC system_header" only gets undone at the end of the file.
//  This issue is fixed in -std=c++2a
#       if (__cplusplus < 201709L)
#               pragma GCC system_header
#       endif

#undef  TLOG_DEBUG
#define TLOG_DEBUG(lvl,...) TRACE_STREAMER(((TLVL_DEBUG+lvl)<64)?TLVL_DEBUG+lvl:63, \
										  tlog_ARG2(not_used, ##__VA_ARGS__,0,need_at_least_one), \
										  tlog_ARG3(not_used, ##__VA_ARGS__,0,"",need_at_least_one), \
										  1, 0 )

#endif // LOGGING_INCLUDE_LOGGING_LOGGER_HPP_
