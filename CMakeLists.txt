cmake_minimum_required(VERSION 3.12)
project(logging VERSION 1.1.2)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(TRACE REQUIRED)
find_package(ers REQUIRED)

#message("TRACE_VERSION=${TRACE_VERSION} TRACE_VERSION_MAJOR=${TRACE_VERSION_MAJOR} TRACE_VERSION_MINOR=${TRACE_VERSION_MINOR} TRACE_VERSION_PATCH=${TRACE_VERSION_PATCH} TRACE_INCLUDE_DIRS=${TRACE_INCLUDE_DIRS} *** TRACE_INCLUDE_DIR=${TRACE_INCLUDE_DIR}")

if(TARGET TRACE::TRACE) # check for more modern cmake methodology in TRACE
  #message("TRACE::TRACE target defined")
  set(LOGGING_DEPENDENCIES ers::ers TRACE::TRACE)  # pthread
else()
  #message("TRACE::TRACE target NOT defined - include_directories")
  set(LOGGING_DEPENDENCIES ers::ers)  # pthread
  if(${TRACE_VERSION} VERSION_EQUAL "3.17.03" AND TRACE_INCLUDE_DIR)
    include_directories("${TRACE_INCLUDE_DIR}")  # adds to the list; singular TRACE_INCLUDE_DIR should not usually be used in CMakeLists.txt files
  endif()
endif()

daq_add_library( LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )


daq_add_application( exception_example exception_example.cxx TEST LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )
daq_add_application( basic_functionality_example basic_functionality_example.cxx TEST LINK_LIBRARIES logging )
daq_add_application( performance performance.cxx TEST LINK_LIBRARIES logging )
daq_add_application( six_streams six_streams.cxx TEST LINK_LIBRARIES logging )
daq_add_application( debug_speed debug_speed.cxx TEST LINK_LIBRARIES logging )
daq_add_application( multithreaded_warning_log multithreaded_warning_log.cxx TEST LINK_LIBRARIES logging )
daq_add_application( log_progress_update log_progress_update.cxx TEST LINK_LIBRARIES logging )


daq_install()
