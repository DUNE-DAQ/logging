/**
 * @file Logger.hpp logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef LOGGING_INCLUDE_LOGGING_LOGGING_HPP_
#define LOGGING_INCLUDE_LOGGING_LOGGING_HPP_

#include "TRACE/trace.h"
#include "ers/OutputStream.hpp"
#include "ers/ers.hpp"
#include <string>
#include <vector>

#include "logging/internal/macro.hpp"

namespace dunedaq::logging {
/**
 * @brief The Logger class defines the interface necessary to configure central
 * logging within a DAQ Application.
 */
class Logging
{
public:
  /**
   * @brief Setup the Logger service
   * Currently no args.
   */
  static void setup(void)
  {
    // need to get tricky to short circuit DEBUG message (at "level 1") about
    //    libmtsStreams.so: cannot open shared object file: No such file or directory
    /** Example: production env:
            export DUNEDAQ_ERS_ERROR="erstrace,throttle(30,100),lstderr,mts"
            export DUNEDAQ_ERS_FATAL="erstrace,lstderr,mts"
            export DUNEDAQ_ERS_WARNING="erstrace,throttle(30,100),lstderr,mts"
            export DUNEDAQ_ERS_INFO="erstrace,throttle(30,100),lstdout,mts"

            export DUNEDAQ_ERS_LOG="lstdout"
            export DUNEDAQ_ERS_DEBUG="lstdout"

            export DUNEDAQ_ERS_STREAM_LIBS="mtsStreams"
     */
    // set defaults with 0=no override
    setenv("DUNEDAQ_ERS_FATAL", "erstrace,lstderr", 0);
    setenv("DUNEDAQ_ERS_ERROR", "erstrace,throttle(30,100),lstderr", 0);
    setenv("DUNEDAQ_ERS_WARNING", "erstrace,throttle(30,100),lstderr", 0);
    setenv("DUNEDAQ_ERS_INFO", "erstrace,lstdout", 0);
    setenv("DUNEDAQ_ERS_LOG", "lstdout", 0);
    setenv("DUNEDAQ_ERS_DEBUG", "lstdout", 0);
    std::vector<std::string> envvars = {
      "DUNEDAQ_ERS_FATAL", "DUNEDAQ_ERS_ERROR", "DUNEDAQ_ERS_WARNING", "DUNEDAQ_ERS_INFO"
    };
    for (std::string& envvar : envvars) {
      char* ecp = getenv(envvar.c_str());
      if (strncmp(ecp, "erstrace", 8) != 0) {
        std::string newval = "erstrace," + std::string(ecp);
        setenv(envvar.c_str(), newval.c_str(), 1);
      }
    }

    // setenv("DUNEDAQ_APPLICATION_NAME","XYZZY",0);
    //  std::ostringstream out;
    //  out << (1ULL<<(TLVL_DEBUG+2))-1;
    //  setenv("TRACE_LVLM",(out.str()+",0").c_str(),0);

    // Avoid ERS_INTERNAL_DEBUG( 1, "Library " << MRSStreamLibraryName << " can not be loaded because " << ex.reason() )
    //  DEBUG_1 [ers::PluginManager::PluginManager(...) at ...Library mtsStreams can not be loaded because
    //  libmtsStreams.so: cannot open shared object file: No such file or directory
    // by only setting debug_level AFTER first ers::debug message
    // setenv("DUNEDAQ_ERS_DEBUG_LEVEL","63",0);
    ers::LocalContext lc("logging package", __FILE__, __LINE__, __func__, 0 /*no_stack*/);
    int lvl = 100;
    ers::InternalMessage msg(lc,
                             "Logger setup(...) ers::debug level " + std::to_string(lvl) +
                               " -- seems to come out level 0 (with ERS version v0_26_00d) ???");
    msg.set_severity(ers::Severity(ers::Debug, lvl));
    ers::debug(msg, lvl); // still comes out as level 0 ???
    ers::Configuration::instance().debug_level(63);
    char* cp;
    if ((cp = getenv("DUNEDAQ_ERS_DEBUG_LEVEL")) && *cp) {
      int lvl = strtoul(cp, nullptr, 0) + TLVL_DEBUG;
      if (lvl > 63)
        lvl = 63;
      // TRACE_CNTL("lvlmskSg",(1ULL<<lvl)-1); // this sets traceTID to id of "Logger"
      uint64_t msk = ((1ULL << lvl) - 1) | (1ULL << lvl);
      std::string mskstr = std::to_string(msk);
      setenv("TRACE_LVLS", mskstr.c_str(), 0);
    }
  }
};

} // namespace logging

#include "logging/detail/Logger.hxx"

//  The following uses gnu extension of "##" connecting "," with empty __VA_ARGS__
//  which eats "," when __VA_ARGS__ is empty.
//      pragma GCC diagnostic ignored "-Wvariadic-macros" // doesn't seem to work.
//  Neither does pragma GCC diagnostic ignored "-Wpedantic" // g++ -std=c++11 --help=warnings | egrep -i
//  'variadic|pedantic' These are last because "pragma GCC system_header" only gets undone at the end of the file. This
//  issue is fixed in -std=c++2a
#if (__cplusplus < 201709L)
#pragma GCC system_header
#endif

#if TRACE_REVNUM <= 1443
#undef TLOG_DEBUG
#define TLOG_DEBUG(lvl, ...)                                                                                           \
  TRACE_STREAMER(((TLVL_DEBUG + lvl) < 64) ? TLVL_DEBUG + lvl : 63,                                                    \
                 tlog_ARG2(not_used, ##__VA_ARGS__, 0, need_at_least_one),                                             \
                 tlog_ARG3(not_used, ##__VA_ARGS__, 0, "", need_at_least_one),                                         \
                 1,                                                                                                    \
                 0)
#endif

#endif // LOGGING_INCLUDE_LOGGING_LOGGING_HPP_
