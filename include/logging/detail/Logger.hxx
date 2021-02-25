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


/*  verstrace_user
    Only log and debug "levels" (or "streams") are supported
 */
#if defined(__has_feature)
#  if __has_feature(thread_sanitizer)
__attribute__((no_sanitize("thread")))
#  endif
#endif
static void verstrace_user(struct timeval */*tvp*/, int TID, uint8_t lvl, const char* insert
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
			printed = TRACE_SNPRINTED(retval,sizeof(obuf));
		}
		if (nargs) {
			retval = vsnprintf(&(obuf[printed]), sizeof(obuf) - printed, msg, ap); // man page say obuf will always be terminated
			printed += TRACE_SNPRINTED(retval,sizeof(obuf)-printed);
		} else {
			/* don't do any parsing for format specifiers in the msg -- tshow will
			   also know to do this on the memory side of things */
			retval = snprintf( &(obuf[printed]), sizeof(obuf)-printed, "%s", msg );
			printed += TRACE_SNPRINTED(retval,sizeof(obuf)-printed);
		}
		if (obuf[printed-1] == '\n')
			obuf[printed-1] = '\0';  // DONE w/ printed (don't need to decrement
		outp = obuf;
	} else {
		if (msg[strlen(msg)-1] == '\n') { // need to copy to remove the trailing nl
			retval = snprintf( obuf, sizeof(obuf), "%s", msg );
			printed = TRACE_SNPRINTED(retval,sizeof(obuf));
			if (obuf[printed-1] == '\n')
				obuf[printed-1] = '\0';  // DONE w/ printed (don't need to decrement
			outp = obuf;
		} else
			outp = msg;
	}
	// LocalContext args: 1-"package_name" 2-"file" 3-"line" 4-"pretty_function" 5-"include_stack"
	ers::LocalContext lc(
						 reinterpret_cast<char*>(idx2namsPtr(TID)),
						 file, line, function, DEBUG_FORCED );
	//std::ostringstream ers_report_impl_out_buffer;
	//ers_report_impl_out_buffer << outp;
	if (lvl < TLVL_DEBUG) { // NOTE: at least currently, TLVL_LOG is numerically 1 less than TLVL_DEBUG
		//auto d = std::chrono::seconds{tvp->tv_sec} + std::chrono::microseconds{tvp->tv_usec};
		////std::chrono::system_clock::time_point tp{std::chrono::duration_cast<std::chrono::system_clock::duration>(d)};
		//std::chrono::system_clock::time_point tp{d};
		// PROTECTED - ers::Issue iss( ers::Issue( ers::Severity(ers::Log), tp, lc, outp, std::vector<std::string>(), std::map<std::string,std::string>(), nullptr ));
		ers::log(   ers::InternalMessage(lc,outp) );
	} else {
		ers::debug( ers::InternalMessage(lc,outp),lvl-TLVL_DEBUG );
	}
}

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



// The following allow an ers::Issue to be streamed into TLOG() or TLOG_DEBUG(N)
inline void operator<<(TraceStreamer& x, const ers::Issue &r)
{
    if (x.do_m) {
		x.line_ = r.context().line_number();
		x.msg_append(r.message().c_str());
		// MAY NEED TO APPEND CHAINED ISSUE???
		// const ers::Issue *issp = &r;
		// while ((issp=issp->cause())) {
		// }
	}
	if (x.do_s) {
		if      (x.lvl_==TLVL_INFO) ers::info(  r );					\
		else if (x.lvl_==TLVL_LOG)  ers::log(   r );					\
		else                        ers::debug( r, x.lvl_-TLVL_DEBUG );	\
		x.do_s = 0;
	}
}

inline void operator<<(TraceStreamer& x, const ers::InternalMessage &r)
{
	if (x.do_m) {
		x.line_ = r.context().line_number();
		x.msg_append(r.message().c_str());
		// MAY NEED TO APPEND CHAINED ISSUE???
	}
	if (x.do_s) {
		if      (x.lvl_==TLVL_INFO) ers::info(  r );					\
		else if (x.lvl_==TLVL_LOG)  ers::log(   r );					\
		else                        ers::debug( r, x.lvl_-TLVL_DEBUG );	\
		x.do_s = 0;
	}
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
// DUNEDAQ_ERS_{FATAL,ERROR,WARNING} environment variables.
#include <ers/OutputStream.h>
namespace ers
{
struct erstraceStream : public OutputStream {
        void write( const ers::Issue & issue )
        {
			ers::Severity sev = issue.severity();
			uint8_t lvl_=TLVL_DEBUG;
			switch (sev.type) {
			case ers::Debug:       lvl_=TLVL_DEBUG+sev.rank;break;
			case ers::Log:         lvl_=TLVL_LOG;           break;
			case ers::Information: lvl_=TLVL_INFO;          break;
			case ers::Warning:     lvl_=TLVL_WARNING;       break;
			case ers::Error:       lvl_=TLVL_ERROR;         break;
			case ers::Fatal:       lvl_=TLVL_FATAL;         break;
			}
			struct { char tn[TRACE_TN_BUFSZ]; } _trc_;
			if (TRACE_INIT_CHECK(trace_name(TRACE_NAME,issue.context().file_name(),_trc_.tn,sizeof(_trc_.tn)))) {
				if (traceControl_rwp->mode.bits.M && (traceLvls_p[traceTID].M & TLVLMSK(lvl_))) {
					struct timeval lclTime;
					std::chrono::system_clock::time_point tp{issue.ptime()};
					//auto micros = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch());
					std::chrono::microseconds micros = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch());
					lclTime.tv_sec  = micros.count() / 1000000;
					lclTime.tv_usec = micros.count() % 1000000;
					int traceID = trace_name2TID( trace_name(TRACE_NAME,issue.context().file_name(),_trc_.tn,sizeof(_trc_.tn)) );
					std::string complete_message = issue.message();
					const ers::Issue *issp = &issue;
					while ((issp=issp->cause())) {
						char fbuf[0x100], tbuf[0x40];
						int strip_ns=1;
						tp = issp->ptime();
						std::chrono::milliseconds millis
							= std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
						struct tm tm_s;
						time_t secs  = millis.count() / 1000;
						localtime_r(&secs, &tm_s);
						if (strftime(tbuf, sizeof(tbuf), "%Y-%b-%d %H:%M:%S", &tm_s) == 0)
							tbuf[0]= '\0';
						complete_message += "\n\tcaused by: " + std::string(tbuf);
						int milliseconds = millis.count() % 1000;
						sprintf(tbuf,",%03d ",milliseconds);
						complete_message += std::string(tbuf);
						trace_func_to_short_func(issp->context().function_name(), fbuf, sizeof(fbuf), strip_ns);
						complete_message += ers::to_string(issp->severity())
							+ " [" + fbuf + " at "
							+ trace_path_components(issp->context().file_name(),0)
							+ ":" + std::to_string(issp->context().line_number())
							+ "] " + issp->message();
					}
					trace(&lclTime, traceID, lvl_, issue.context().line_number(),
					      issue.context().function_name(),
					      0 TRACE_XTRA_PASSED, complete_message.c_str());
				}
            }
			chained().write( issue );
        }
};
}
ERS_REGISTER_OUTPUT_STREAM( ers::erstraceStream, "erstrace", ERS_EMPTY )    // last param is "param" 


// Support macros
#define SL_FRC(lvl) ((lvl)>=0 && (lvl)<TLVL_DEBUG)

#define LOG0_()              
#define LOG1_(value)            value,
#define LOG2_(value, value1)    value , value1,
#define COMMA_IF_PARENS(...)  ,
#define LPAREN                (
#define EXPAND_(...)           __VA_ARGS__
#define CHOOSE_(...)           EXPAND_(LOG__  LPAREN \
									 __VA_ARGS__ COMMA_IF_PARENS __VA_ARGS__ COMMA_IF_PARENS __VA_ARGS__ (), \
									 LOG2_, impossible, LOG2_, LOG1_, LOG0_, LOG1_, ))
#define LOG__(a0, a1, a2, a3, a4, a5, arg, ...) arg
#define _tlog_ARG2( a1,a2,...)    tlog_ARG2(a1,a2,__VA_ARGS__)
#define _tlog_ARG3( a1,a2,a3,...) tlog_ARG3(a1,a2,a3,__VA_ARGS__)


#endif // LOGGING_INCLUDE_LOGGING_DETAIL_LOGGER_HXX_
