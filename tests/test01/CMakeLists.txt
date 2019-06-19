###############################################################################
# Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Expect bugs
#
# Please use and enjoy. Please let me know of any bugs/improvements
# that you have found/implemented and I will fix/incorporate them into this
# file.
###############################################################################

if (MSVC AND USE_STATIC_CRT)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()


if (NOT BUILD_SHARED_LIBS)
	# Use ppx static library
	add_definitions(-DPPXBASE_STATIC)
endif()


add_executable(
	test01-server
	WIN32
	server.cpp
	ipc_test.h
	ipc_test.cpp
	)
	
add_executable(
	test01-client
	WIN32
	client.cpp
	ipc_test.h
	ipc_test.cpp
	)


# Win32 Console
set_target_properties(test01-server PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
set_target_properties(test01-server PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")

set_target_properties(test01-client PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
set_target_properties(test01-client PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")


find_package(GTest MODULE REQUIRED)
target_link_libraries(test01-client GTest::GTest GTest::Main)
target_link_libraries(test01-server GTest::GTest GTest::Main)


if(BUILD_SHARED_LIBS)
	# link ppxbase.lib
	target_link_libraries(test01-client 
		$<TARGET_LINKER_FILE:ppxbase> )
		
	target_link_libraries(test01-server 
		$<TARGET_LINKER_FILE:ppxbase> )
else()
	# link ppxbase-static.lib
	target_link_libraries(test01-client 
		$<TARGET_LINKER_FILE:ppxbase-static> )

	target_link_libraries(test01-server 
		$<TARGET_LINKER_FILE:ppxbase-static> )
endif()

