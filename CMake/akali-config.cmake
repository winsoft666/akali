# akali-config.cmake - package configuration file

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(EXISTS ${SELF_DIR}/akali-target.cmake)
	include(${SELF_DIR}/akali-target.cmake)
endif()

if(EXISTS ${SELF_DIR}/akali-static-target.cmake)
	include(${SELF_DIR}/akali-static-target.cmake)
endif()
