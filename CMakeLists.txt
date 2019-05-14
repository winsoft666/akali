cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(ppx CXX)

configure_file (
  "${PROJECT_SOURCE_DIR}/include/ppx_config.h.in"
  "${PROJECT_SOURCE_DIR}/include/ppx_config.h"
  )

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(PPX_NO_IPC "Without IPC module" ON)
option(PPX_NO_ENCRYPT "Without ENCRYPT module" ON)
option(PPX_NO_HTTP "Without HTTP module" ON)

if(WIN32)
	option(USE_STATIC_CRT "Set to ON to build with static CRT on Windows (/MT)." ON)
endif()

add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)

if(BUILD_SHARED_LIBS)
	add_definitions(-DPPX_EXPORTS)
else()
	add_definitions(-DPPX_STATIC)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

