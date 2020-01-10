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

set (CMAKE_CXX_STANDARD 11)

if(BUILD_SHARED_LIBS)
	set(LIB_NAME akali)
	set(AKALI_STATIC NO)
else()
	set(LIB_NAME akali-static)
	set(AKALI_STATIC YES)
endif()

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



file(GLOB ROOT_HEADER_FILES 			../include/*.h ../include/*.hpp)
file(GLOB BASE_HEADER_FILES 			../include/akali/*.h ../include/akali/*.hpp)

file(GLOB BASE_SOURCE_FILES 			./*.cpp)


source_group("Source Files" 		    FILES ${BASE_SOURCE_FILES})

source_group("Header Files" 			FILES ${ROOT_HEADER_FILES})
source_group("Header Files\\akali"   	FILES ${BASE_HEADER_FILES})


if(BUILD_SHARED_LIBS)
	add_definitions(-DAKALI_EXPORTS)

	add_library(
		${LIB_NAME}
		SHARED
		${BASE_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${BASE_HEADER_FILES}
	)
	
	# set output name prefix/suffix
	set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}
		DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})

else()
	add_definitions(-DAKALI_STATIC)
	
	add_library(
		${LIB_NAME}
		STATIC
		${BASE_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${BASE_HEADER_FILES}
	)
	
	# set output name prefix/suffix
    set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}
		DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
		
		
	set_target_properties(${LIB_NAME} PROPERTIES 
		INTERFACE_COMPILE_DEFINITIONS AKALI_STATIC)
endif()

if(WIN32)
	set_target_properties(${LIB_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")

	target_link_libraries(${LIB_NAME} PUBLIC 
		Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib Psapi.lib Shlwapi.lib)
endif()

# Set output directory
set_target_properties(${LIB_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

target_include_directories(${LIB_NAME} INTERFACE
	$<INSTALL_INTERFACE:include>)


install(TARGETS ${LIB_NAME}
	EXPORT ${LIB_NAME}-target
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
	


install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/akali-config.cmake 
	DESTINATION ${AKALI_INSTALL_CMAKE_DIR})

install(FILES ../include/akali.h 
	DESTINATION include)

install(FILES ${BASE_HEADER_FILES} 
	DESTINATION include/akali)

export(TARGETS ${LIB_NAME}
       APPEND FILE ${PROJECT_BINARY_DIR}/${LIB_NAME}-target.cmake
)

install(EXPORT ${LIB_NAME}-target
        FILE ${LIB_NAME}-target.cmake
        DESTINATION ${AKALI_INSTALL_CMAKE_DIR}
)

