#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/vars.cmake)

function(list_backend_dependency_includes INCLUDES)
	file(GLOB files LIST_DIRECTORIES false ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/backends/*.cmake)
	set(${INCLUDES} ${files} PARENT_SCOPE)
endfunction()

list_backend_dependency_includes(BACKEND_INCLUDES)

set(${PROJECT_NAME}_BACKENDS)

foreach(INC ${BACKEND_INCLUDES})
	get_filename_component(BACKEND_NAME ${INC} NAME_WE)
	string(TOUPPER ${BACKEND_NAME} BACKEND_NAME_UC)

	set(OPTION_NAME ${PROJECT_NAME_UC}_WITH_${BACKEND_NAME_UC})
	option(${OPTION_NAME} "Enable ${BACKEND_NAME} backend" ON)

	set(${PROJECT_NAME}_HAVE_${BACKEND_NAME_UC} false)
	if(${OPTION_NAME})
		include(${INC})
		if(${BACKEND_NAME}_FOUND)
			list(APPEND ${PROJECT_NAME}_BACKENDS ${BACKEND_NAME})
			set(${PROJECT_NAME}_HAVE_${BACKEND_NAME_UC} true)
			message(STATUS "Enabled ${BACKEND_NAME} backend")
		else()
			message(STATUS "Skipped ${BACKEND_NAME} backend because ${BACKEND_NAME}_FOUND is false")
		endif()
	else()
		message(STATUS "Skipped ${BACKEND_NAME} backend because option ${OPTION_NAME} is set to ${${OPTION_NAME}}")
	endif()
endforeach()

if(NOT ${PROJECT_NAME}_BACKENDS)
	message(FATAL_ERROR "No backends configured!")
endif()
