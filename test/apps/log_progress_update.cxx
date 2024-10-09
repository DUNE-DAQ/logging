#ifndef DUNEDAQ_PACKAGE_NAME                     // this could/may be (tbd) set by the build system
#       define DUNEDAQ_PACKAGE_NAME "Logging_"          // becomes an ERS Qualifier
#endif
#include <logging/Logging.hpp>  // NOTE: if ISSUES ARE DECLARED BEFORE include logging/Logging.hpp, TLOG_DEBUG<<issue wont work.
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
  dunedaq::logging::Logging::setup("test", "log_progress_update"); 

  std::atomic<uint64_t> m_generated_tot{ 7 };
  std::vector<int> theList{1,2,3};
  std::ostringstream oss_prog;
  TLOG() << "start";
  oss_prog << "Generated list #" << m_generated_tot.load() << " with contents " << theList
	   << " and size " << theList.size() << ". ";

  std::cout << typeid(ProgressUpdate).name() << "\n\n";
  
  TLOG()       << static_cast<logging::ProgressUpdate>(ProgressUpdate(ERS_HERE, "someName1", oss_prog.str()));

#if 1
  TLOG_DEBUG() << ProgressUpdate(ERS_HERE, "someName2", oss_prog.str());
  std::cout    << ProgressUpdate(ERS_HERE, "someName3", oss_prog.str()) << '\n';

  TLOG_DEBUG() << ProgressUpdate(ERS_HERE, "someName4", oss_prog.str());
  TLOG_DEBUG() << ProgressUpdate(ERS_HERE, "someName5", oss_prog.str());
#endif
  
  return 0;
}
