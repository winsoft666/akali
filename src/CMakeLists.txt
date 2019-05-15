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



file(GLOB ROOT_HEADER_FILES 			../include/*.h)
file(GLOB BASE_HEADER_FILES 			../include/base/*.h)
file(GLOB ENCRYPT_HEADER_FILES 			../include/encrypt/*.h)
file(GLOB NET_HEADER_FILES 				../include/net/*.h)

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
	
	set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME}
											 DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
	
	if(NOT PPX_NO_HTTP)
		find_package(CURL REQUIRED)

		target_link_libraries(${LIB_NAME} PRIVATE ${CURL_LIBRARIES})
		target_include_directories(${LIB_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
	endif()


	if(NOT PPX_NO_ENCRYPT)
		find_package(cryptopp CONFIG REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE cryptopp)
	endif()


	if(NOT PPX_NO_IPC)
		find_package(msgpack CONFIG REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE msgpackc msgpackc msgpackc-cxx)
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
	
    set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME}${STATIC_LIBNAME_SUFFIX}
												 DEBUG_OUTPUT_NAME ${LIB_NAME}${STATIC_LIBNAME_SUFFIX}${DEBUG_LIBNAME_SUFFIX})
	
	
	if(NOT PPX_NO_HTTP)
		find_package(CURL REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE ${CURL_LIBRARIES})
		target_include_directories(${LIB_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
	endif()
	

	if(NOT PPX_NO_ENCRYPT)
		find_package(cryptopp CONFIG REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE cryptopp-static)
	endif()


	if(NOT PPX_NO_IPC)
		find_package(msgpack CONFIG REQUIRED)
		target_link_libraries(${LIB_NAME} PRIVATE msgpackc msgpackc-cxx msgpackc-static)
	endif()
endif()


target_link_libraries(${LIB_NAME} PRIVATE Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib)


install(TARGETS ${LIB_NAME}
	 RUNTIME DESTINATION bin
	 LIBRARY DESTINATION lib
	 ARCHIVE DESTINATION lib)

install(FILES ${ROOT_HEADER_FILES} 			DESTINATION include)
install(FILES ${BASE_HEADER_FILES} 			DESTINATION include/base)
install(FILES ${ENCRYPT_HEADER_FILES} 		DESTINATION include/encrypt)
install(FILES ${NET_HEADER_FILES} 			DESTINATION include/net)





