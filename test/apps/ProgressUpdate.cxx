#ifndef DUNEDAQ_PACKAGE_NAME                     // this could/may be (tbd) set by the build system
#       define DUNEDAQ_PACKAGE_NAME "Logging_"          // becomes an ERS Qualifier
#endif
#include <logging/Logging.hpp>
#include <ers/Issue.hpp>


ERS_DECLARE_ISSUE_BASE(dfmodules,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))


int main(/*int argc, char *argv[]*/)
{
  setenv("DUNEDAQ_APPLICATION_NAME","LOGGING_PROGRESS_APP",0);
  dunedaq::logging::Logging::setup(); // not strictly needed -- checks/establishes a default env.

  std::vector<int> theList{1,2,3};
  std::ostringstream oss_prog;
  oss_prog << "Generated list #" << 3 << " with contents " << theList
	   << " and size " << theList.size() << ". ";
  TLOG() << (ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));

  return 0;
}
