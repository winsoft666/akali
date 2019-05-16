set(LIB_NAME ppx)


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
file(GLOB BASE_HEADER_FILES 			../include/base/*.h ../include/base/*.hpp)
file(GLOB ENCRYPT_HEADER_FILES 			../include/encrypt/*.h ../include/encrypt/*.hpp)
file(GLOB NET_HEADER_FILES 				../include/net/*.h ../include/net/*.hpp)

file(GLOB ROOT_SOURCE_FILES 			./*.cpp)
file(GLOB BASE_SOURCE_FILES 			./base/*.cpp)
file(GLOB ENCRYPT_SOURCE_FILES 			./encrypt/*.cpp)
file(GLOB NET_SOURCE_FILES 				./net/*.cpp)


source_group("Source Files" 			FILES ${ROOT_SOURCE_FILES})
source_group("Source Files\\base" 		FILES ${BASE_SOURCE_FILES})
source_group("Source Files\\net" 		FILES ${NET_SOURCE_FILES})
source_group("Source Files\\encrypt" 	FILES ${ENCRYPT_SOURCE_FILES})

source_group("Header Files" 			FILES ${ROOT_HEADER_FILES})
source_group("Header Files\\base"   	FILES ${BASE_HEADER_FILES})
source_group("Header Files\\net"     	FILES ${NET_HEADER_FILES})
source_group("Header Files\\encrypt" 	FILES ${ENCRYPT_HEADER_FILES})

set(_LINK_LIBRARYS "")

if(BUILD_SHARED_LIBS)
	add_definitions(-DPPX_EXPORTS)
	
	add_library(
		${LIB_NAME}
		SHARED
		${ROOT_SOURCE_FILES}
		${BASE_SOURCE_FILES}
		${ENCRYPT_SOURCE_FILES}
		${NET_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${BASE_HEADER_FILES}
		${NET_HEADER_FILES}
		${ENCRYPT_HEADER_FILES}
	)
	
	# set output name prefix/suffix
	set_target_properties(${LIB_NAME} PROPERTIES 
						OUTPUT_NAME ${LIB_NAME} )
	
	if(NOT PPX_NO_HTTP)
		find_package(CURL REQUIRED)

		target_link_libraries(${LIB_NAME} PRIVATE ${CURL_LIBRARIES})
		target_include_directories(${LIB_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
		list(APPEND _LINK_LIBRARYS ${CURL_LIBRARIES})
	endif()


	if(NOT PPX_NO_ENCRYPT)
		find_package(cryptopp CONFIG REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE cryptopp-static)
		list(APPEND _LINK_LIBRARYS cryptopp-static)
	endif()
else()
	add_definitions(-DPPX_STATIC -DCURL_STATICLIB)
	
	add_library(
		${LIB_NAME}
		STATIC
		${ROOT_SOURCE_FILES}
		${BASE_SOURCE_FILES}
		${ENCRYPT_SOURCE_FILES}
		${NET_SOURCE_FILES}
		${ROOT_HEADER_FILES}
		${BASE_HEADER_FILES}
		${NET_HEADER_FILES}
		${ENCRYPT_HEADER_FILES}
	)
	
	# set output name prefix/suffix
    set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}${STATIC_LIBNAME_SUFFIX})
	
	if(NOT PPX_NO_HTTP)
		find_package(CURL REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE ${CURL_LIBRARIES})
		target_include_directories(${LIB_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
		list(APPEND _LINK_LIBRARYS ${CURL_LIBRARIES})
	endif()

	if(NOT PPX_NO_ENCRYPT)
		find_package(cryptopp CONFIG REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE cryptopp-static)
		list(APPEND _LINK_LIBRARYS cryptopp-static)
	endif()
endif()

target_link_libraries(${LIB_NAME} PRIVATE 
	Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib)
list(APPEND _LINK_LIBRARYS Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib)

# Set output directory
set_target_properties(${LIB_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

set_property(GLOBAL PROPERTY PPX_LINK_LIBRARYS "${_LINK_LIBRARYS}" )


install(TARGETS ${LIB_NAME}
	 RUNTIME DESTINATION bin
	 LIBRARY DESTINATION lib
	 ARCHIVE DESTINATION lib)

install(FILES ${ROOT_HEADER_FILES} 			DESTINATION include)
install(FILES ${BASE_HEADER_FILES} 			DESTINATION include/base)
install(FILES ${ENCRYPT_HEADER_FILES} 		DESTINATION include/encrypt)
install(FILES ${NET_HEADER_FILES} 			DESTINATION include/net)

