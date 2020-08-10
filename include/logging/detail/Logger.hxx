/**
 * @file Logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef LOGGING_INCLUDE_LOGGING_DETAIL_LOGGER_HXX_
#define LOGGING_INCLUDE_LOGGING_DETAIL_LOGGER_HXX_

#include <stdlib.h>				// setenv

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
	//std::ostringstream ers_report_impl_out_buffer;
	//ers_report_impl_out_buffer << outp;
	switch (static_cast<lvl_t>(lvl))
	{
	case lvl_t::TLVL_INFO:
		ers::info( ers::Message( lc, outp )	\
		         BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY( ERS_EMPTY ERS_EMPTY ) ) ) ERS_EMPTY ); \
		break;
 	case lvl_t::TLVL_LOG:
		ers::log( ers::Message( lc, outp )	\
		         BOOST_PP_COMMA_IF( BOOST_PP_NOT( ERS_IS_EMPTY( ERS_EMPTY ERS_EMPTY ) ) ) ERS_EMPTY ); \
		break;
	default:
		//lvl -=2 ;
		if ( ers::debug_level() >= lvl )
			ers::debug( ers::Message( lc, outp ) \
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
	class ErsInfoStreamer
	{
	public:
		//ErsInfoStreamer( ers::LocalContext c ) : _ctx(c) {}
		//ers::LocalContext _ctx;
		inline ErsInfoStreamer &operator<<(const ers::Issue &r)
		{
			// would be nice of the "context" could be created from the streamer instance
			ers::warning( r );
			return *this;
		}
	};  // class ErsInfoStreamer
	class ErsLogStreamer
	{
	public:
		//ErsLogStreamer( ers::LocalContext c ) : _ctx(c) {}
		//ers::LocalContext _ctx;
		inline ErsLogStreamer &operator<<(const ers::Issue &r)
		{
			// would be nice of the "context" could be created from the streamer instance
			ers::warning( r );
			return *this;
		}
	};  // class ErsLogStreamer
}  // namespace ""



// The following allows "erstrace" to be included in the ERS configuration via
// TDAQ_ERS_{FATAL,ERROR,WARNING} environment variables.
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
# if TRACE_REVNUM >= 1322
                              issue.context().function_name(),
# endif
                              0 TRACE_XTRA_PASSED, issue.message().c_str());
                }
                chained().write( issue );
        }
};
}
ERS_REGISTER_OUTPUT_STREAM( ers::erstraceStream, "erstrace", ERS_EMPTY )    // last param is "param" 


// Support macros
#define SL_FRC(lvl) ((lvl)==0 || (lvl)==1)

#define LOG0_()              
#define LOG1_(value)            value,
#define LOG2_(value, value1)    value , value1,
#define COMMA_IF_PARENS(...)  ,
#define LPAREN                (
#define EXPAND(...)           __VA_ARGS__
#define CHOOSE(...)           EXPAND(LOG__  LPAREN \
									 __VA_ARGS__ COMMA_IF_PARENS __VA_ARGS__ COMMA_IF_PARENS __VA_ARGS__ (), \
									 LOG2_, impossible, LOG2_, LOG1_, LOG0_, LOG1_, ))
#define LOG__(a0, a1, a2, a3, a4, a5, arg, ...) arg
#define _tlog_ARG2( a1,a2,...)    tlog_ARG2(a1,a2,__VA_ARGS__)
#define _tlog_ARG3( a1,a2,a3,...) tlog_ARG3(a1,a2,a3,__VA_ARGS__)


#endif // LOGGING_INCLUDE_LOGGING_DETAIL_LOGGER_HXX_
