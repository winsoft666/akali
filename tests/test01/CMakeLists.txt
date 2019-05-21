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
	add_definitions(-DPPX_STATIC)
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

# Include vcpkg include directory, is any best way?
target_include_directories(test01-server PRIVATE 
	${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)

target_include_directories(test01-client PRIVATE 
	${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)


if(BUILD_SHARED_LIBS)

	# link ppxbase.lib
	target_link_libraries(test01-client 
		$<TARGET_LINKER_FILE:ppxbase> )
		
	target_link_libraries(test01-server 
		$<TARGET_LINKER_FILE:ppxbase> )
else()
	# link libppxbase.lib
	target_link_libraries(test01-client 
		$<TARGET_LINKER_FILE:ppxbase> )

	target_link_libraries(test01-server 
		$<TARGET_LINKER_FILE:ppxbase> )
endif()

