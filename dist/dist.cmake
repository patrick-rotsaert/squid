#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

find_program(CPACK_COMMAND cpack REQUIRED)
find_program(NINJA_COMMAND ninja REQUIRED)

if(NOT BUILD_DIR)
	set(BUILD_DIR /tmp/squid-build)
endif()

set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")

function(exec)
	list(APPEND ARGN COMMAND_ERROR_IS_FATAL ANY)
	if(DEFINED COMMAND_ECHO)
		list(APPEND ARGN COMMAND_ECHO ${COMMAND_ECHO})
	endif()
	execute_process(${ARGN})
endfunction()

function(configure BUILD_DIR BUILD_SHARED_LIBS CMAKE_BUILD_TYPE)
	exec(
		COMMAND ${CMAKE_COMMAND}
			-G Ninja
			-S ${SOURCE_DIR} -B ${BUILD_DIR}
			-DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	)
endfunction()

configure(${BUILD_DIR}/shared-release YES Release)
configure(${BUILD_DIR}/shared-debug YES Debug)
configure(${BUILD_DIR}/static-release NO Release)
configure(${BUILD_DIR}/static-debug NO Debug)

function(build BUILD_DIR)
	exec(COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR})
endfunction()

build(${BUILD_DIR}/shared-release)
build(${BUILD_DIR}/shared-debug)
build(${BUILD_DIR}/static-release)
build(${BUILD_DIR}/static-debug)

configure_file(
	${CMAKE_CURRENT_LIST_DIR}/package.cmake.in
	${BUILD_DIR}/package.cmake
	@ONLY
)

exec(
	COMMAND ${CPACK_COMMAND} --config package.cmake
	WORKING_DIRECTORY ${BUILD_DIR}
)
