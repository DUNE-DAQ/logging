/**
 * @file Logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef LOGGING_INCLUDE_LOGGING_LOGGER_HPP_
#define LOGGING_INCLUDE_LOGGING_LOGGER_HPP_

#if !defined(TDAQ_PACKAGE_NAME) && defined(TRACE_NAME)
#	define TDAQ_PACKAGE_NAME TRACE_NAME
#elif !defined(TRACE_NAME) && defined(TDAQ_PACKAGE_NAME)
#	define TRACE_NAME TDAQ_PACKAGE_NAME
#endif

#include "ers/ers.h"
#include <ers/OutputStream.h>
#define TRACE_LOG_FUN_PROTO \
  static void erstrace_user(struct timeval *, int, uint8_t, const char*, const char*, int, const char*, uint16_t nargs, const char *msg, ...); \
  static void erstrace_user(struct timeval *, int, uint8_t, const char*, const char*, int, const char*, uint16_t nargs, const std::string& msg, ...)
#undef TRACE_LOG_FUNCTION
#define TRACE_LOG_FUNCTION erstrace_user
#define TRACE_4_LVLSTRS "fatal","error","warning","info"
#define TRACE_60_LVLSTRS  "log","debug","dbg06","dbg07","dbg08","dbg09","dbg10","dbg11","dbg12","dbg13","dbg14","dbg15",\
		"dbg16","dbg17","dbg18","dbg19","dbg20","dbg21","dbg22","dbg23","dbg24","dbg25","dbg26","dbg27","dbg28","dbg29","dbg30","dbg31",\
		"dbg32","dbg33","dbg34","dbg35","dbg36","dbg37","dbg38","dbg39","dbg40","dbg41","dbg42","dbg43","dbg44","dbg45","dbg46","dbg47",\
		"dbg48","dbg49","dbg50","dbg51","dbg52","dbg53","dbg54","dbg55","dbg56","dbg57","dbg58","dbg59","dbg60","dbg61","dbg62","dbg63"
#include "TRACE/trace.h"


// The following from TRACE will become enum
#undef TLVL_ERROR
#undef TLVL_WARNING
#undef TLVL_INFO
#undef TLVL_DEBUG

enum class lvl_t { TLVL_FATAL,TLVL_ERROR,TLVL_WARNING,TLVL_INFO,TLVL_LOG,TLVL_DEBUG,
		d00=5,d01=5,d02=5,d03=5,d04=5,d05=5,d06,d07,d08,d09,d10,d11,d12,d13,d14,d15,
		d16,d17,d18,d19,d20,d21,d22,d23,d24,d25,d26,d27,d28,d29,d30,d31,
		d32,d33,d34,d35,d36,d37,d38,d39,d40,d41,d42,d43,d44,d45,d46,d47,
		d48,d49,d50,d51,d52,d53,d54,d55,d56,d57,d58,d59,d60,d61,d62,d63};

#include <logging/detail/Logger.hxx> // needs TLVL_* definitions


#undef ERS_DECLARE_ISSUE_BASE_HPP
#define ERS_DECLARE_ISSUE_BASE_HPP( namespace_name, class_name, base_class_name, message_, base_attributes, attributes ) \
        __ERS_DECLARE_ISSUE_BASE__( namespace_name, class_name, base_class_name, ERS_EMPTY message_, ERS_EMPTY base_attributes, ERS_EMPTY attributes )\
		static inline TraceStreamer& operator<<(TraceStreamer& x, const namespace_name::class_name &r) \
		{if (x.do_m)   { x.line_=r.context().line_number(); x.msg_append( r.message().c_str() );} \
		 switch(static_cast<lvl_t>(x.lvl_)){							\
		 /*case TLVL_ERROR:   ers::error(   r ); x.do_s = 0; break;*/ \
		 /*case lvl_t::TLVL_WARNING: ers::warning( r ); x.do_s = 0; break;*/ \
		 case lvl_t::TLVL_INFO:    ers::info(    r ); x.do_s = 0; break;	\
		 case lvl_t::TLVL_LOG:     ers::log(     r ); x.do_s = 0; break;	\
		 default:                  ers::debug(   r,x.lvl_ ); x.do_s = 0; break; \
		 }																\
		 return x; \
		}


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
		setenv("TDAQ_ERS_FATAL", "erstrace,lstderr",0);
		setenv("TDAQ_ERS_ERROR", "erstrace,throttle(30,100),lstderr",0);
		setenv("TDAQ_ERS_WARNING","erstrace,throttle(30,100),lstderr",0);
		setenv("TDAQ_ERS_INFO",   "lstdout",0);
		setenv("TDAQ_ERS_LOG",    "lstdout",0);
		setenv("TDAQ_ERS_DEBUG",  "lstdout",0);
		//setenv("TDAQ_APPLICATION_NAME","XYZZY",0);

		// Avoid DEBUG_1 [ers::PluginManager::PluginManager(...) at ...Library mtsStreams can not be loaded because libmtsStreams.so: cannot open shared object file: No such file or directory
		// by only setting debug_level AFTER first ers::debug message
		//setenv("TDAQ_ERS_DEBUG_LEVEL","63",0);
		ers::LocalContext lc( "unknown", __FILE__, __LINE__, __func__, 0/*no_stack*/ );
		int lvl=1;
		ers::debug(ers::Message(lc,"hello") BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY(ERS_EMPTY lvl) ) ) lvl);
		ers::Configuration::instance().debug_level(63);
	}
};




#define LOG_FATAL(...)   ErsFatalStreamer()
#define LOG_ERROR(...)   ErsErrorStreamer()
#define LOG_WARNING(...) ErsWarningStreamer()


#define LOG_INFO(...)      TRACE_STREAMER(static_cast<int>(lvl_t::TLVL_INFO), \
										  _tlog_ARG2(not_used, CHOOSE(__VA_ARGS__)(__VA_ARGS__) 0,need_at_least_one), \
										  _tlog_ARG3(not_used, CHOOSE(__VA_ARGS__)(__VA_ARGS__) 0,"",need_at_least_one), \
										  1, SL_FRC(static_cast<int>(lvl_t::TLVL_INFO)) )
#define LOG_LOG(...)       TRACE_STREAMER(static_cast<int>(lvl_t::TLVL_LOG), \
										  _tlog_ARG2(not_used, CHOOSE(__VA_ARGS__)(__VA_ARGS__) 0,need_at_least_one), \
										  _tlog_ARG3(not_used, CHOOSE(__VA_ARGS__)(__VA_ARGS__) 0,"",need_at_least_one), \
										  1, SL_FRC(static_cast<int>(lvl_t::TLVL_LOG)) )

//  The following uses gnu extension of "##" connecting "," with empty __VA_ARGS__
//  which eats "," when __VA_ARGS__ is empty.
//      pragma GCC diagnostic ignored "-Wvariadic-macros" // doesn't seem to work.
//  Neither does pragma GCC diagnostic ignored "-Wpedantic" // g++ -std=c++11 --help=warnings | egrep -i 'variadic|pedantic'
//  These are last because "pragma GCC system_header" only gets undone at the end of the file.
//  This issue is fixed in -std=c++2a
#       if (__cplusplus < 201709L)
#               pragma GCC system_header
#       endif

#define LOG_DEBUG(lvl,...) TRACE_STREAMER(static_cast<int>(lvl_t::lvl),	\
										  tlog_ARG2(not_used, ##__VA_ARGS__,0,need_at_least_one), \
										  tlog_ARG3(not_used, ##__VA_ARGS__,0,"",need_at_least_one), \
										  1, SL_FRC(static_cast<int>(lvl_t::lvl)) )

#endif // LOGGING_INCLUDE_LOGGING_LOGGER_HPP_
