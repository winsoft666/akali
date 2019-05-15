cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

project(ppx CXX)

configure_file (
  "${PROJECT_SOURCE_DIR}/include/ppx_config.h.in"
  "${PROJECT_SOURCE_DIR}/include/ppx_config.h"
  )

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(PPX_NO_IPC "Without IPC module" ON)
option(PPX_NO_ENCRYPT "Without ENCRYPT module" ON)
option(PPX_NO_HTTP "Without HTTP module" ON)
option(USE_STATIC_CRT "Set to ON to build with static CRT on Windows (/MT)." OFF)


if(WIN32)
	# Dll default use MD(d), Lib default use MT(d)
	if(BUILD_SHARED_LIBS)
		set(USE_STATIC_CRT OFF)
	else()
		set(USE_STATIC_CRT ON)
	endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Debug Output
message(STATUS "BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}")
message(STATUS "USE_STATIC_CRT=${USE_STATIC_CRT}")
message(STATUS "PPX_NO_IPC=${PPX_NO_IPC}")
message(STATUS "PPX_NO_ENCRYPT=${PPX_NO_ENCRYPT}")
message(STATUS "PPX_NO_HTTP=${PPX_NO_HTTP}")
message(STATUS "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")


set(STATIC_LIBNAME_SUFFIX "-static" 
	CACHE STRING "Optional suffix to append to the library name for a static build")


# Unicode Support
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)


include_directories(./include)



add_subdirectory(src)

add_subdirectory(tests)