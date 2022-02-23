#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_guard(GLOBAL)

function(list_backend_dependency_includes INCLUDES)
	file(GLOB files LIST_DIRECTORIES false ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/backends/*.cmake)
	set(${INCLUDES} ${files} PARENT_SCOPE)
endfunction()

list_backend_dependency_includes(BACKEND_INCLUDES)

set(SQUID_BACKENDS)

foreach(file ${BACKEND_INCLUDES})
	get_filename_component(BACKEND_NAME ${file} NAME_WE)
	string(TOUPPER ${BACKEND_NAME} BACKEND_NAME_UC)

	option(SQUID_WITH_${BACKEND_NAME_UC} "Enable ${BACKEND_NAME} backend" ON)

	set(SQUID_HAVE_${BACKEND_NAME_UC} false)
	if(SQUID_WITH_${BACKEND_NAME_UC})
		include(${file})
		if(${BACKEND_NAME}_FOUND)
			list(APPEND SQUID_BACKENDS ${BACKEND_NAME})
			set(SQUID_HAVE_${BACKEND_NAME_UC} true)
			message(STATUS "Enabled ${BACKEND_NAME} backend")
		else()
			message(STATUS "Skipped ${BACKEND_NAME} backend because ${BACKEND_NAME}_FOUND is false")
		endif()
	else()
		message(STATUS "Skipped ${BACKEND_NAME} backend because option SQUID_WITH_${BACKEND_NAME_UC} is set to ${SQUID_WITH_${BACKEND_NAME_UC}}")
	endif()
endforeach()

if(NOT SQUID_BACKENDS)
	message(FATAL_ERROR "No backends configured!")
endif()
