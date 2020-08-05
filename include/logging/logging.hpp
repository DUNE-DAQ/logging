/**
 * @file logger.hpp logger interface definition
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
#include <stdlib.h>				// setenv


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


#if defined(__has_feature)
#  if __has_feature(thread_sanitizer)
__attribute__((no_sanitize("thread")))
#  endif
#endif
static void verstrace_user(struct timeval *, int TID, uint8_t lvl, const char* insert
	, const char* file, int line, const char* function, uint16_t nargs, const char *msg, va_list ap)
{
	size_t printed = 0;
	int    retval;
	const char *outp;
	char   obuf[TRACE_USER_MSGMAX];

	if ((insert && (printed = strlen(insert))) || nargs)
	{
		/* check insert 1st to make sure printed is set */
		// assume insert is smaller than obuf
		if (printed) {
			retval = snprintf(obuf,sizeof(obuf),"%s ",insert );
			printed = SNPRINTED(retval,sizeof(obuf));
		}
		if (nargs) {
			retval = vsnprintf(&(obuf[printed]), sizeof(obuf) - printed, msg, ap); // man page say obuf will always be terminated
			printed += SNPRINTED(retval,sizeof(obuf)-printed);
		} else {
			/* don't do any parsing for format specifiers in the msg -- tshow will
			   also know to do this on the memory side of things */
			retval = snprintf( &(obuf[printed]), sizeof(obuf)-printed, "%s", msg );
			printed += SNPRINTED(retval,sizeof(obuf)-printed);
		}
		if (obuf[printed-1] == '\n')
			obuf[printed-1] = '\0';  // DONE w/ printed (don't need to decrement
		outp = obuf;
	} else {
		if (msg[strlen(msg)-1] == '\n') { // need to copy to remove the trailing nl
			retval = snprintf( obuf, sizeof(obuf), "%s", msg );
			printed = SNPRINTED(retval,sizeof(obuf));
			if (obuf[printed-1] == '\n')
				obuf[printed-1] = '\0';  // DONE w/ printed (don't need to decrement
			outp = obuf;
		} else
			outp = msg;
	}
	// LocalContext args: 1-"package_name" 2-"file" 3-"line" 4-"pretty_function" 5-"include_stack"
	ers::LocalContext lc( traceNamLvls_p[TID].name, file, line, function, DEBUG_FORCED );
	std::ostringstream ers_report_impl_out_buffer;
	ers_report_impl_out_buffer << outp;
	switch (static_cast<lvl_t>(lvl))
	{
	case lvl_t::TLVL_INFO:
		ers::info( ers::Message( lc, ers_report_impl_out_buffer.str() )	\
		         BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY( ERS_EMPTY ERS_EMPTY ) ) ) ERS_EMPTY ); \
		break;
 	case lvl_t::TLVL_LOG:
		ers::log( ers::Message( lc, ers_report_impl_out_buffer.str() )	\
		         BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY( ERS_EMPTY ERS_EMPTY ) ) ) ERS_EMPTY ); \
		break;
	default:
		//lvl -=2 ;
		if ( ers::debug_level() >= lvl )
			ers::debug( ers::Message( lc, ers_report_impl_out_buffer.str() ) \
			           BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY( ERS_EMPTY lvl ) ) ) lvl ); \
		break;
	}
}

// NOTE: lvl translation occurs
SUPPRESS_NOT_USED_WARN
static void erstrace_user(struct timeval *tvp, int TID, uint8_t lvl, const char* insert, const char* file, int line, const char* function, uint16_t nargs, const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	verstrace_user(tvp, TID, lvl, insert, file, line, function, nargs, msg, ap);
	va_end(ap);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvarargs"
SUPPRESS_NOT_USED_WARN
static void erstrace_user(struct timeval *tvp, int TID, uint8_t lvl, const char* insert, const char* file, int line, const char* function, uint16_t nargs, const std::string& msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	verstrace_user(tvp, TID, lvl, insert, file, line, function, nargs, &msg[0], ap);
	va_end(ap);
}   /* trace */
#pragma GCC diagnostic pop



// The following allow an ers::Issue to be streamed into LOG_DEBUG(N)
inline TraceStreamer& operator<<(TraceStreamer& x, const ers::Issue &r)
{
	//assert(x.lvl_ == TLVL_WARING)
	if (x.do_m) {
		x.line_ = r.context().line_number();
		x.msg_append(r.message().c_str());
	}
	if (static_cast<lvl_t>(x.lvl_) == lvl_t::TLVL_WARNING)
		ers::warning( r );
	else
		ers::info( r );
	x.do_s = 0;
	return x;
}

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



namespace {  // unnamed namespace (i.e. static (for each compliation unit only))

	class ErsFatalStreamer
	{
	public:
		inline ErsFatalStreamer &operator<<(const ers::Issue &r)
		{
			ers::fatal( r );
			return *this;
		}
	};  // class ErsFatalStreamer

	class ErsErrorStreamer
	{
	public:
		inline ErsErrorStreamer &operator<<(const ers::Issue &r)
		{
			ers::error( r );
			return *this;
		}
	};  // class ErsErrorStreamer

	class ErsWarningStreamer
	{
	public:
		//ErsWarningStreamer( ers::LocalContext c ) : _ctx(c) {}
		//ers::LocalContext _ctx;
		inline ErsWarningStreamer &operator<<(const ers::Issue &r)
		{
			// would be nice of the "context" could be created from the streamer instance
			ers::warning( r );
			return *this;
		}
	};  // class ErsWarningStreamer
}  // namespace ""




#include <ers/OutputStream.h>
namespace ers
{
struct erstraceStream : public OutputStream {
        void write( const ers::Issue & issue )
        {
                // can't get "severity" that the stream is associated with.
                // what about the time??
                if TRACE_INIT_CHECK(TRACE_NAME)
                {
                        struct timeval lclTime;
						ers::Severity sev = issue.severity();
                        uint8_t lvl_;
						switch (sev.type) {
						case ers::Debug:       lvl_=5+sev.rank; break;
						case ers::Log:         lvl_=4;          break;
						case ers::Information: lvl_=3;          break;
						case ers::Warning:     lvl_=2;          break;
						case ers::Error:       lvl_=1;          break;
						case ers::Fatal:       lvl_=0;          break;
						}
                        std::chrono::system_clock::time_point tp{issue.ptime()};
                        //auto micros = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch());
                        std::chrono::microseconds micros = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch());
                        lclTime.tv_sec  = micros.count() / 1000000;
                        lclTime.tv_usec = micros.count() % 1000000;
						int traceID = name2TID(trace_path_components(issue.context().file_name(),1));
                        trace(&lclTime, traceID, lvl_, issue.context().line_number(),
# if TRACE_HAS_FUNCTION
                              issue.context().function_name(),
# endif
                              0 TRACE_XTRA_PASSED, issue.message().c_str());
                }
                chained().write( issue );
        }
};
}
ERS_REGISTER_OUTPUT_STREAM( ers::erstraceStream, "erstrace", ERS_EMPTY )    // last param is "param" 


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
		setenv("TDAQ_ERS_DEBUG_LEVEL","63",0);
	}
};




#define LOG_FATAL(...)   ErsFatalStreamer()
#define LOG_ERROR(...)   ErsErrorStreamer()
#define LOG_WARNING(...) ErsWarningStreamer()


#define SL_FRC(lvl) ((lvl)==0 || (lvl)==1)

#define LOG0()              
#define LOG1(value)            value,
#define LOG2(value, value1)    value , value1,
#define COMMA_IF_PARENS(...)  ,
#define LPAREN                (
#define EXPAND(...)           __VA_ARGS__
#define CHOOSE(...)           EXPAND(LOG LPAREN \
									 __VA_ARGS__ COMMA_IF_PARENS __VA_ARGS__ COMMA_IF_PARENS __VA_ARGS__ (), \
									 LOG2, impossible, LOG2, LOG1, LOG0, LOG1, ))
#define LOG(a0, a1, a2, a3, a4, a5, arg, ...) arg
#define _tlog_ARG2( a1,a2,...)    tlog_ARG2(a1,a2,__VA_ARGS__)
#define _tlog_ARG3( a1,a2,a3,...) tlog_ARG3(a1,a2,a3,__VA_ARGS__)

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
