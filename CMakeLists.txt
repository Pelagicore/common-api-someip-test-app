cmake_minimum_required(VERSION 2.6)

set(PACKAGE test-commonapi)
PROJECT(${PACKAGE})

# Dependencies
FIND_PACKAGE(CommonAPICodeGen REQUIRED)
FIND_PACKAGE(IVILogging REQUIRED)
FIND_PACKAGE(PkgConfig REQUIRED)

set(prefix ${CMAKE_INSTALL_PREFIX})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

if (ENABLE_SYSTEMD)
	add_definitions(-DENABLE_SYSTEMD)
	set(SYSTEMD_CONFIGURATION_FILES_DIR "/lib/systemd/system")
endif()

add_subdirectory (src)

