cmake_minimum_required(VERSION 3.12)
project(logging)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

find_package(TRACE 3.15.09 REQUIRED)
find_package(ers REQUIRED)
#set(LOGGING_DEPENDENCIES ers::ers pthread)
include_directories(${ers_DIR}/../../../include)
#message("include_directories(${ers_DIR}/../../../include)")

point_build_to( test )
add_executable( basic_functionality_example test/basic_functionality_example.cxx )
target_include_directories(basic_functionality_example PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> )
target_link_libraries( basic_functionality_example ${ers_DIR}/../../../lib64/libers.so pthread )

daq_install( VERSION 1.0.0 TARGETS basic_functionality_example )
