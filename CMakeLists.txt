cmake_minimum_required(VERSION 3.12)
project(logging VERSION 0.0.0)

find_package(daq-buildtools REQUIRED)

daq_setup_environment()

find_package(TRACE 3.15.09 REQUIRED)
find_package(ers REQUIRED)


set(LOGGING_DEPENDENCIES ers::ers)  # pthread

add_library( logging INTERFACE )
target_link_libraries( logging INTERFACE ${LOGGING_DEPENDENCIES} )
target_include_directories( logging INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> )
_daq_define_exportname()
install(TARGETS logging EXPORT ${DAQ_PROJECT_EXPORTNAME} )

daq_add_application( basic_functionality_example basic_functionality_example.cxx TEST LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )
install(TARGETS basic_functionality_example EXPORT ${DAQ_PROJECT_EXPORTNAME} )

daq_add_application( performance performance.cxx TEST LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )
install(TARGETS performance EXPORT ${DAQ_PROJECT_EXPORTNAME} )

daq_add_application( six_streams six_streams.cxx TEST LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )
install(TARGETS six_streams EXPORT ${DAQ_PROJECT_EXPORTNAME} )

daq_add_application( debug_speed debug_speed.cxx TEST LINK_LIBRARIES ${LOGGING_DEPENDENCIES} )
install(TARGETS debug_speed EXPORT ${DAQ_PROJECT_EXPORTNAME} )


daq_install()
