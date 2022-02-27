#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)

include(CMakePackageConfigHelpers)

if(NOT DEFINED ${PROJECT_NAME}_INSTALL_CMAKEDIR)
	set(${PROJECT_NAME}_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "Path to ${PROJECT_NAME} CMake files")
endif()

if(BUILD_SHARED_LIBS)
	set(type shared)
else()
	set(type static)
endif()

install(EXPORT ${PROJECT_NAME}_Targets
	DESTINATION "${${PROJECT_NAME}_INSTALL_CMAKEDIR}"
	NAMESPACE ${PROJECT_NAME}::
	FILE ${PROJECT_NAME}-${type}-targets.cmake
	COMPONENT development
)

write_basic_package_version_file(
    ${PROJECT_NAME}-config-version.cmake
    COMPATIBILITY SameMajorVersion
)

# @PROJECT_DEPENDENCIES@ is referenced by cmake/project-config.cmake.in
set(PROJECT_DEPENDENCIES ${${PROJECT_NAME}_BACKENDS})

configure_file(
	${PROJECT_BASE_DIR}/cmake/project-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	@ONLY
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	DESTINATION "${${PROJECT_NAME}_INSTALL_CMAKEDIR}"
	COMPONENT development
)

