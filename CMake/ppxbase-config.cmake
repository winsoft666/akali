# ppxbase-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(CMAKE_BUILD_TYPE STREQUAL "" OR NOT DEFINED CMAKE_BUILD_TYPE)
	include(${SELF_DIR}/ppxbase-target.cmake)
else()
	include(${SELF_DIR}/ppxbase-target-${CMAKE_BUILD_TYPE}.cmake)
endif()

if(TARGET ppxbase)
	get_target_property(PPXBASE_INCLUDE_DIRS ppxbase INTERFACE_INCLUDE_DIRECTORIES)
	get_target_property(PPXBASE_LIBRARIES ppxbase "LOCATION${CMAKE_BUILD_TYPE}")
endif()