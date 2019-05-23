# ppxbase-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)


include(${SELF_DIR}/ppxbase-target.cmake)


if(TARGET ppxbase)
	get_target_property(PPXBASE_INCLUDE_DIRS ppxbase INTERFACE_INCLUDE_DIRECTORIES)
	get_target_property(PPXBASE_LIBRARIES ppxbase "IMPORTED_IMPLIB_${CMAKE_BUILD_TYPE}")
endif()