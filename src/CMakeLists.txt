set(LIB_NAME ppxbase)


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
file(GLOB BASE_HEADER_FILES 			../include/ppxbase/*.h ../include/ppxbase/*.hpp)

file(GLOB BASE_SOURCE_FILES 			./*.cpp)


source_group("Source Files" 		    FILES ${BASE_SOURCE_FILES})

source_group("Header Files" 			FILES ${ROOT_HEADER_FILES})
source_group("Header Files\\ppxbase"   	FILES ${BASE_HEADER_FILES})


if(BUILD_SHARED_LIBS)
	add_definitions(-DPPXBASE_EXPORTS)
	set(PPXBASE_STATIC NO)
	
	add_library(
		${LIB_NAME}
		SHARED
		${BASE_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${BASE_HEADER_FILES}
	)
	
	# set output name prefix/suffix
	set_target_properties(${LIB_NAME} PROPERTIES 
		DEBUG_OUTPUT_NAME ${LIB_NAME})
	
else()
	add_definitions(-DPPXBASE_STATIC)
	set(PPXBASE_STATIC YES)
	
	add_library(
		${LIB_NAME}
		STATIC
		${BASE_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${BASE_HEADER_FILES}
	)
	
	# set output name prefix/suffix
    set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}${STATIC_LIBNAME_SUFFIX})
		
	set_target_properties(${LIB_NAME} PROPERTIES 
		INTERFACE_COMPILE_DEFINITIONS PPXBASE_STATIC)
endif()

set_target_properties (${LIB_NAME} PROPERTIES 
	DEBUG_POSTFIX ${DEBUG_LIBNAME_SUFFIX})

target_link_libraries(${LIB_NAME} PRIVATE 
	Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib)


# Set output directory
set_target_properties(${LIB_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )


target_include_directories(${LIB_NAME} INTERFACE
	$<INSTALL_INTERFACE:include>)

install(TARGETS ${LIB_NAME}
	EXPORT ppxbase-target
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
	
export(TARGETS ${LIB_NAME}
       APPEND FILE ${PROJECT_BINARY_DIR}/ppxbase-target.cmake
       NAMESPACE PPXBASE::
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../CMake/ppxbase-config.cmake 
	DESTINATION ${PPXBASE_INSTALL_CMAKE_DIR})

install(EXPORT ppxbase-target
        FILE ppxbase-target.cmake
        DESTINATION ${PPXBASE_INSTALL_CMAKE_DIR}
)

install(FILES ../include/ppx_base.h 
	DESTINATION include)

install(FILES ${BASE_HEADER_FILES} 
	DESTINATION include/ppxbase)

