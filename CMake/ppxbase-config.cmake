# ppxbase-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(EXISTS ${SELF_DIR}/ppxbase-target.cmake)
	include(${SELF_DIR}/ppxbase-target.cmake)
endif()

if(EXISTS ${SELF_DIR}/ppxbase-static-target.cmake)
	include(${SELF_DIR}/ppxbase-static-target.cmake)
endif()
