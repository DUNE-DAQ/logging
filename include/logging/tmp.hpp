/**
 * @file tmp.hpp Logger ERS Issue definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef LOGGING_INCLUDE_LOGGING_TMP_HPP_
#define LOGGING_INCLUDE_LOGGING_TMP_HPP_

#include <string>

namespace dunedaq::logging {
class Logging::InternalMessage : public ers::Issue
{
  template<class>
  friend class ers::IssueRegistrator;

public:
  static const char* get_uid() { return "dunedaq::logging::Logging::InternalMessage"; }
  Logging::InternalMessage(const ers::Context& context);
  Logging::InternalMessage(const ers::Context& context, const std::string& msg);
  Logging::InternalMessage(const ers::Context& context, const std::string& msg, const std::exception& cause);
  Logging::InternalMessage(const ers::Context& context, const std::exception& cause);
  void raise() const { throw Logging::InternalMessage(*this); }
  const char* get_class_name() const { return get_uid(); }
  ers::Issue* clone() const { return new dunedaq::logging::Logging::InternalMessage(*this); }
};
} // namespace dunedaq::logging
namespace dunedaq::logging {
inline Logging::InternalMessage::Logging::InternalMessage(const ers::Context& context)
  : ers::Issue(context)
{}
inline Logging::InternalMessage::Logging::InternalMessage(const ers::Context& context, const std::string& msg)
  : ers::Issue(context, msg)
{}
inline Logging::InternalMessage::Logging::InternalMessage(const ers::Context& context,
                                                          const std::string& msg,
                                                          const std::exception& cause)
  : ers::Issue(context, msg, cause)
{}
inline Logging::InternalMessage::Logging::InternalMessage(const ers::Context& context, const std::exception& cause)
  : ers::Issue(context, cause)
{}
} // namespace dunedaq::logging
namespace {
ers::IssueRegistrator<dunedaq::logging::Logging::InternalMessage> dunedaq::logging_Logging::InternalMessage_instance;
} // namespace ""
static inline TraceStreamer&
operator<<(TraceStreamer& x, const dunedaq::logging::Logging::InternalMessage& r)
{
  if (x.do_m) {
    x.line_ = r.context().line_number();
    x.msg_append(r.message().c_str());
  }
  if (x.do_s) {
    if (x.lvl_ == TLVL_INFO)
      ers::info(r);
    else if (x.lvl_ == TLVL_LOG)
      ers::log(r);
    else
      ers::debug(r, x.lvl_ - TLVL_DEBUG);
    x.do_s = 0;
  }
  return x;
}

#endif // LOGGING_INCLUDE_LOGGING_TMP_HPP_