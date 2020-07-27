/**
 * @file logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef LOGGING_INCLUDE_LOGGING_LOGGER_HPP_
#define LOGGING_INCLUDE_LOGGING_LOGGER_HPP_

#include "ers/ers.h"
#include <ers/OutputStream.h>
#define TRACE_LOG_FUN_PROTO \
  static void erstrace_user(struct timeval *, int, uint8_t, const char*, const char*, int, const char*, uint16_t nargs, const char *msg, ...); \
  static void erstrace_user(struct timeval *, int, uint8_t, const char*, const char*, int, const char*, uint16_t nargs, const std::string& msg, ...)
#undef TRACE_LOG_FUNCTION
#define TRACE_LOG_FUNCTION erstrace_user
#define TRACE_4_LVLSTRS "ftl","err","wrn","nfo"
#	define TRACE_60_LVLSTRS  "log", "dbg", "d01", "d02", "d03", "d04", "d05", "d06", "d07", "d08", "d09", \
							 "d10", "d11", "d12", "d13", "d14", "d15", "d16", "d17", "d18", "d19", \
							 "d20", "d21", "d22", "d23", "d24", "d25", "d26", "d27", "d28", "d29", \
							 "d30", "d31", "d32", "d33", "d34", "d35", "d36", "d37", "d38", "d39", \
							 "d40", "d41", "d42", "d43", "d44", "d45", "d46", "d47", "d48", "d49", \
							 "d50", "d51", "d52", "d53", "d54", "d55", "d56", "d57", "d58"
#include "TRACE/trace.h"


#undef TLVL_ERROR
//#undef TLVL_WARNING
#undef TLVL_INFO
#undef TLVL_DEBUG
//#undef TLVL_TRACE
// out with the defaults (above), in with the specifics (below)
#define TLVL_INFO  0   // to be shifted up by 3 later
#define TLVL_LOG   1   // to be shifted up by 3 later
#define TLVL_DEBUG 2   // to be shifted up by 3 later


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

	printf("here lvl=%u\n",lvl);
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
	ers::LocalContext lc( traceNamLvls_p[TID].name, file, line, function, DEBUG_FORCED );
	std::ostringstream ers_report_impl_out_buffer;
	ers_report_impl_out_buffer << outp;
	switch (lvl)
	{
	case TLVL_INFO:
		ers::info( ers::Message( lc, ers_report_impl_out_buffer.str() )	\
		         BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY( ERS_EMPTY ERS_EMPTY ) ) ) ERS_EMPTY ); \
		break;
 	case TLVL_LOG:
		ers::log( ers::Message( lc, ers_report_impl_out_buffer.str() )	\
		         BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY( ERS_EMPTY ERS_EMPTY ) ) ) ERS_EMPTY ); \
		break;
	default:
		lvl -=2 ;
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
	verstrace_user(tvp, TID, lvl-3, insert, file, line, function, nargs, msg, ap);
	va_end(ap);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvarargs"
SUPPRESS_NOT_USED_WARN
static void erstrace_user(struct timeval *tvp, int TID, uint8_t lvl, const char* insert, const char* file, int line, const char* function, uint16_t nargs, const std::string& msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	verstrace_user(tvp, TID, lvl-3, insert, file, line, function, nargs, &msg[0], ap);
	va_end(ap);
}   /* trace */
#pragma GCC diagnostic pop




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
#define SL_FRC(lvl) ((lvl)==0 || (lvl)==1)
//  The following use gnu extension of "##" connecting "," with empty __VA_ARGS__
//  which eats "," when __VA_ARGS__ is empty.
//      pragma GCC diagnostic ignored "-Wvariadic-macros" // doesn't seem to work.
//  Neither does pragma GCC diagnostic ignored "-Wpedantic" // g++ -std=c++11 --help=warnings | egrep -i 'variadic|pedantic'
//  These are last because "pragma GCC system_header" only get undone at the end of the file.
//  This issue is fix in -std=c++2a
#       if (__cplusplus < 201709L)
#               pragma GCC system_header
#       endif

#define LOG_DEBUG(lvl,...) TRACE_STREAMER(TLVL_DEBUG+lvl+3,			\
										  tlog_ARG2(not_used, ##__VA_ARGS__,0,need_at_least_one), \
										  tlog_ARG3(not_used, ##__VA_ARGS__,0,"",need_at_least_one), \
										  1, SL_FRC(TLVL_DEBUG+lvl) )


#endif // LOGGING_INCLUDE_LOGGING_LOGGER_HPP_
