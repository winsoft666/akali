set(EXE_NAME test01)


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
	${EXE_NAME}
	WIN32
	main.cpp
	)
	
# Win32 Console
set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")


if(BUILD_SHARED_LIBS)
	target_link_libraries(${EXE_NAME} 
		$<TARGET_LINKER_FILE:ppx> )
else()
	get_property(PPX_LINK_LIBRARYS GLOBAL PROPERTY "PPX_LINK_LIBRARYS" ) 

	target_link_libraries(${EXE_NAME} 
		$<TARGET_LINKER_FILE:ppx> ${PPX_LINK_LIBRARYS})

	# target_link_directories(${EXE_NAME} PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
endif()

