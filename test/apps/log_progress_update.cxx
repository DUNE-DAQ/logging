#ifndef DUNEDAQ_PACKAGE_NAME                     // this could/may be (tbd) set by the build system
#       define DUNEDAQ_PACKAGE_NAME "Logging_"          // becomes an ERS Qualifier
#endif
#include <logging/Logging.hpp>
#include <ers/Issue.hpp>
#include <vector>

ERS_DECLARE_ISSUE(appfwk,                 ///< Namespace
                  GeneralDAQModuleIssue,  ///< Issue class name
                  " DAQModule: " << name, ///< Message
                  ((std::string)name)     ///< Message parameters
)

ERS_DECLARE_ISSUE_BASE(logging,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))

using namespace logging;

std::ostream&
operator<<(std::ostream& t, std::vector<int> ints)
{
  t << "{";
  bool first = true;
  for (auto& i : ints) {
    if (!first)
      t << ", ";
    first = false;
    t << i;
  }
  return t << "}";
}

int main(/*int argc, char *argv[]*/)
{
  setenv("DUNEDAQ_APPLICATION_NAME","LOGGING_PROGRESS_APP",0);
  dunedaq::logging::Logging::setup(); // not strictly needed -- checks/establishes a default env.

  std::atomic<uint64_t> m_generated_tot{ 3 };
  std::vector<int> theList{1,2,3};
  std::ostringstream oss_prog;
  oss_prog << "Generated list #" << m_generated_tot.load() << " with contents " << theList
	   << " and size " << theList.size() << ". ";
  TLOG_DEBUG() << (ProgressUpdate(ERS_HERE, "someName", oss_prog.str()));

  return 0;
}
