#ifndef LOGGING_INCLUDE_LOGGING_INTERNAL_MACRO_HPP_
#define LOGGING_INCLUDE_LOGGING_INTERNAL_MACRO_HPP_

#include "ers/internal/IssueDeclarationMacro.hpp"
#include "ers/internal/macro.hpp"

#undef TRACE_LOG_FUNCTION
#define TRACE_LOG_FUNCTION erstrace_user

// don't worry about ERS_PACKAGE and/or TRACE_NAME -- leave them to the user or build system

//-----------------------------------------------------------------------------


/** 6 logging "streams" are defined: fatal, error, warning, info, log and debug.
    The first 4 are accessed via the ers:: methods with a TRACE destination added.
    THe last 2 are accessed via TRACE macros with an ers:: user method configured.
 */
// Redefine TRACE's TLOG to only take 2 optional args: name and format control
#undef TLOG
#if TRACE_REVNUM <= 1443
# define TLOG(...)  TRACE_STREAMER(TLVL_LOG, \
                                   _tlog_ARG2(not_used, CHOOSE_(__VA_ARGS__)(__VA_ARGS__) 0,need_at_least_one), \
                                   _tlog_ARG3(not_used, CHOOSE_(__VA_ARGS__)(__VA_ARGS__) 0,"",need_at_least_one), \
                                   1, 1)
#else
# define TLOG(...)  TRACE_STREAMER(TLVL_LOG, TLOG2(__VA_ARGS__), 1)
#endif

// TRACE's TLOG_DEBUG maybe OK, depending on the version of TRACE - check at the end of this file
// TLOG_DBG ???

#undef TLOG_ERROR
#undef TLOG_WARNING
#undef TLOG_INFO
#undef TLOG_TRACE
#undef TLOG_ARB

#undef ERS_DEBUG
#undef ERS_INFO
#undef ERS_LOG


# undef  ERS_DECLARE_ISSUE_BASE
# define ERS_DECLARE_ISSUE_BASE(           namespace_name, class_name, base_class_name, message_, base_attributes, attributes ) \
        __ERS_DECLARE_ISSUE_BASE__(        namespace_name, class_name, base_class_name, message_, base_attributes, attributes ) \
        __ERS_DEFINE_ISSUE_BASE__( inline, namespace_name, class_name, base_class_name, message_, base_attributes, attributes ) \
	namespace namespace_name {					\
	  static inline TraceStreamer& operator<<(TraceStreamer& x, const class_name &r) \
                {if (x.do_m)   {\
                     x.line_=r.context().line_number(); x.msg_append( r.message().c_str() );\
                     /* MAY NEED TO APPEND CHAINED ISSUE??? */          \
                 }                                                  \
                 if (x.do_s) { \
                     if      (x.lvl_==TLVL_INFO) ers::info(  r );       \
                     else if (x.lvl_==TLVL_LOG)  ers::log(   r );       \
                     else                        ers::debug( r, x.lvl_-TLVL_DEBUG ); \
                     x.do_s = 0;                                        \
                 }                                                      \
                 return x;                                              \
                } \
	}

# undef  ERS_DECLARE_ISSUE
# define ERS_DECLARE_ISSUE(                namespace_name, class_name,                       message_,            attributes ) \
        __ERS_DECLARE_ISSUE_BASE__(        namespace_name, class_name, ers::Issue, ERS_EMPTY message_, ERS_EMPTY, attributes ) \
        __ERS_DEFINE_ISSUE_BASE__( inline, namespace_name, class_name, ers::Issue, ERS_EMPTY message_, ERS_EMPTY, attributes ) \
	namespace namespace_name {					\
	  static inline TraceStreamer& operator<<(TraceStreamer& x, const class_name &r) \
                {if (x.do_m)   {\
                     x.line_=r.context().line_number(); x.msg_append( r.message().c_str() );\
                     /* MAY NEED TO APPEND CHAINED ISSUE??? */          \
                 }                                                  \
                 if (x.do_s) {                                          \
                     if      (x.lvl_==TLVL_INFO) ers::info(  r );       \
                     else if (x.lvl_==TLVL_LOG)  ers::log(   r );       \
                     else                        ers::debug( r, x.lvl_-TLVL_DEBUG ); \
                     x.do_s = 0;                                        \
                 }                                                      \
                 return x; \
                } \
	}

#endif // LOGGING_INCLUDE_LOGGING_INTERNAL_MACRO_HPP_
