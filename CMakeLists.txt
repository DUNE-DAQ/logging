cmake_minimum_required(VERSION 3.12)
project(logging VERSION 1.0.2)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(TRACE 3.16.02 REQUIRED)
find_package(ers REQUIRED)

set(LOGGING_DEPENDENCIES ers::ers)  # pthread

daq_add_library( LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )

daq_add_application( exception_example exception_example.cxx TEST LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )
daq_add_application( basic_functionality_example basic_functionality_example.cxx TEST LINK_LIBRARIES logging )
daq_add_application( performance performance.cxx TEST LINK_LIBRARIES logging )
daq_add_application( six_streams six_streams.cxx TEST LINK_LIBRARIES logging )
daq_add_application( debug_speed debug_speed.cxx TEST LINK_LIBRARIES logging )

daq_add_python_bindings(*.cpp LINK_LIBRARIES ${PROJECT_NAME})

daq_install()
