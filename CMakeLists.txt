cmake_minimum_required(VERSION 3.12)
project(logging VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

find_package(TRACE 3.15.09 REQUIRED)
find_package(ers REQUIRED)
set(LOGGING_DEPENDENCIES ers::ers)  # pthread

add_library( logging INTERFACE )
target_link_libraries( logging INTERFACE ${LOGGING_DEPENDENCIES} )
target_include_directories( logging INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> )

daq_point_build_to( test )
add_executable( basic_functionality_example test/basic_functionality_example.cxx )
target_link_libraries( basic_functionality_example ${LOGGING_DEPENDENCIES} pthread )
add_executable( performance test/performance.cxx )
target_link_libraries( performance ${LOGGING_DEPENDENCIES} )
add_executable( six_streams test/six_streams.cxx )
target_link_libraries( six_streams ${LOGGING_DEPENDENCIES} )
add_executable( debug_speed test/debug_speed.cxx )
target_link_libraries( debug_speed ${LOGGING_DEPENDENCIES} )

daq_install( TARGETS basic_functionality_example six_streams debug_speed logging )
