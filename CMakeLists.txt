cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

project(ppxbase CXX)

configure_file (
  "${PROJECT_SOURCE_DIR}/include/ppx_config.h.in"
  "${PROJECT_SOURCE_DIR}/include/ppx_config.h"
  )

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build tests project" ON)
option(USE_STATIC_CRT "Set to ON to build with static CRT on Windows (/MT)." OFF)


if(WIN32)
	# Dll default use MD(d), Lib default use MT(d)
	if(BUILD_SHARED_LIBS)
		set(USE_STATIC_CRT OFF)
	else()
		set(USE_STATIC_CRT ON)
	endif()
endif()

if(MSVC)
  set(PPXBASE_INSTALL_CMAKE_DIR share/ppxbase)
elseif(WIN32 AND NOT CYGWIN)
  set(PPXBASE_INSTALL_CMAKE_DIR CMake)
else()
  set(PPXBASE_INSTALL_CMAKE_DIR lib/cmake/ppxbase)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Debug Output
message(STATUS "BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}")
message(STATUS "USE_STATIC_CRT=${USE_STATIC_CRT}")
message(STATUS "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")


set(DEBUG_LIBNAME_SUFFIX "-d" 
	CACHE STRING "Optional suffix to append to the library name for a debug build")
mark_as_advanced(DEBUG_LIBNAME_SUFFIX)

# Unicode Support
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)



include_directories(./include)

add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(tests)
endif()