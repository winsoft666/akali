set(LIB_NAME ppx)



if(USE_STATIC_CRT)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
endif()



file(GLOB ROOT_HEADER_FILES 			../include/*.h)
file(GLOB BASE_HEADER_FILES 			../include/base/*.h)
file(GLOB ENCRYPT_HEADER_FILES 			../include/encrypt/*.h)
file(GLOB NET_HEADER_FILES 				../include/net/*.h)

file(GLOB ROOT_SOURCE_FILES 			./*.cpp)
file(GLOB BASE_SOURCE_FILES 			./base/*.cpp)
file(GLOB ENCRYPT_SOURCE_FILES 			./encrypt/*.cpp)
file(GLOB NET_SOURCE_FILES 				./net/*.cpp)


add_library(
	${LIB_NAME}
	${ROOT_SOURCE_FILES}
	${BASE_SOURCE_FILES}
	${ENCRYPT_SOURCE_FILES}
	${NET_SOURCE_FILES}
	)
	
target_include_directories(${LIB_NAME} 
	PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/../include)

                     
source_group("Source Files" 			FILES ${ROOT_SOURCE_FILES})
source_group("Source Files\\base" 		FILES ${BASE_SOURCE_FILES})
source_group("Source Files\\net" 		FILES ${NET_SOURCE_FILES})
source_group("Source Files\\encrypt" 	FILES ${ENCRYPT_SOURCE_FILES})

source_group("Header Files" 			FILES ${ROOT_HEADER_FILES})
source_group("Header Files\\base"   	FILES ${BASE_HEADER_FILES})
source_group("Header Files\\net"     	FILES ${NET_HEADER_FILES})
source_group("Header Files\\encrypt" 	FILES ${ENCRYPT_HEADER_FILES})