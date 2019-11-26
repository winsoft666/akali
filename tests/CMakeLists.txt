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

set(EXE_NAME test02)

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

file(GLOB SOURCE_FILES 			./*.cpp)

add_executable(
	${EXE_NAME}
	WIN32
	${SOURCE_FILES}
	)
	
# Win32 Console
set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")

find_package(GTest MODULE REQUIRED)
target_link_libraries(${EXE_NAME} GTest::GTest GTest::Main)

add_test(${EXE_NAME} ${EXE_NAME})

if(BUILD_SHARED_LIBS)
	add_dependencies(${EXE_NAME} ppxbase)
	
	target_link_libraries(${EXE_NAME} 
		$<TARGET_LINKER_FILE:ppxbase> )
else()
	add_dependencies(${EXE_NAME} ppxbase-static)
	
	target_link_libraries(${EXE_NAME} 
		$<TARGET_LINKER_FILE:ppxbase-static> )

endif()

