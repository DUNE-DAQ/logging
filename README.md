# logging
DUNE DAQ logging package

This package is mainly based on ERS and secondarily based on TRACE.
One link to ERS information can be found [here](https://atlas-tdaq-monitoring.web.cern.ch/OH/refman/ERSHowTo.html).
One link to TRACE information can be found [here](https://cdcvs.fnal.gov/redmine/projects/trace/wiki).
ERS provides:
- Assertion Macros
- Logging Macros<code>*</code>
- Macros for declaring Custom Issues

<code>*</code> TRACE provides high-speed dynamically activated debug logging and is integrated with the ERS logging; new macros are defined.

Users should be able to use the Assertion Macros from ERS and also declare Custom Issues. The issues can be used in logging and exception processing.

For logging, six streamer macros are provided:
1. LOG_FATAL()     << ers::Issue
2. LOG_ERROR()     << ers::Issue
3. LOG_WARNING()   << ers::Issue
4. LOG_INFO()      << ers::Issue or basic string/args
5. LOG_LOG()       << ers::Issue or basic string/args
6. LOG_DEBUG(lvl)  << ers::Issue or basic string/args

~~~cpp
LOG_ERROR() << ers::CantOpenFile2(ERS_HERE,"My_Error_FileName");
~~~
or
~~~cpp
LOG_DEBUG(0) << "general message";
~~~
or
~~~cpp
LOG_DEBUG(0) << ers::Message(ERS_HERE,"My_Warn_Message with ignored macro param");
~~~
