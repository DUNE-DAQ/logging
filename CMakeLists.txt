cmake_minimum_required(VERSION 3.12)
project(logging VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

find_package(TRACE 3.15.09 REQUIRED)
find_package(ers REQUIRED)
set(LOGGING_DEPENDENCIES ers::ers)  # pthread

daq_point_build_to( test )
add_executable( basic_functionality_example test/basic_functionality_example.cxx )
target_link_libraries( basic_functionality_example ${LOGGING_DEPENDENCIES} pthread )
add_executable( performance test/performance.cxx )
target_link_libraries( performance ${LOGGING_DEPENDENCIES} )

daq_install( TARGETS basic_functionality_example )
