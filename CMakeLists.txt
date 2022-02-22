#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

project(Squid CXX)
cmake_minimum_required(VERSION 3.17)

include(GNUInstallDirs)

include(cmake/backends.cmake)
include(cmake/options.cmake)
include(cmake/paths.cmake)
include(cmake/version.cmake)

function(PRIV_add_squid_library TARGET LINKAGE)
	set(OPTIONS)
	set(ONE_VALUE_ARGS)
	set(MULTI_VALUE_ARGS 
		SOURCES PUBLIC_HEADERS PRIVATE_HEADERS
		PUBLIC_LIBRARIES
		PUBLIC_SQUID_LIBRARIES
	)
	cmake_parse_arguments(P "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

	# Set the target output name
	if(WIN32)
		set(TARGET_OUTPUT_NAME "${TARGET}_${SQUID_VERSION_MAJOR}_${SQUID_VERSION_MINOR}")
	else()
		set(TARGET_OUTPUT_NAME ${TARGET})
	endif()

	# Suffix the target name with "_static" for static libs
	set(SUFFIX)
	if(LINKAGE STREQUAL STATIC)
		set(SUFFIX _static)
	endif()
	set(TARGET ${TARGET}${SUFFIX})

	# Add the library target
	add_library(${TARGET} ${LINKAGE} ${P_SOURCES} ${P_PUBLIC_HEADERS} ${P_PRIVATE_HEADERS})
	
	# Hide all symbols by default
	set_target_properties(${TARGET} PROPERTIES CXX_VISIBILITY_PRESET hidden)

	# Specify the C++ standard
	target_compile_features(${TARGET} PRIVATE cxx_std_20)

	# Set the compiler warning options
	target_compile_options(${TARGET} PRIVATE
		"$<$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>:$<BUILD_INTERFACE:-Wall;-Wextra;-pedantic;-Werror>>"
		"$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:$<BUILD_INTERFACE:/W4;/WX>>"
	)

	# Set the common include directory
	target_include_directories(${TARGET} PUBLIC
		$<BUILD_INTERFACE:${SQUID_BASE_DIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)

	# Link libraries
	if(P_PUBLIC_LIBRARIES)
		target_link_libraries(${TARGET} PUBLIC ${P_PUBLIC_LIBRARIES})
	endif()

	# Link Squid libraries
	# Link shared to shared and static to static, hence the suffix.
	foreach(LIB ${P_PUBLIC_SQUID_LIBRARIES})
		target_link_libraries(${TARGET} PUBLIC ${LIB}${SUFFIX})
	endforeach()

	# Set other target properties
	set(PROPERTIES
		OUTPUT_NAME "${TARGET_OUTPUT_NAME}"
		CLEAN_DIRECT_OUTPUT 1
	)
	if(LINKAGE STREQUAL SHARED)
		list(APPEND PROPERTIES  VERSION ${SQUID_VERSION})
		if (UNIX)
			list(APPEND PROPERTIES
				SOVERSION ${SQUID_SOVERSION}
				INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib
			)
		endif()
		target_compile_definitions(${TARGET} 
			PUBLIC SQUID_SHARED 
			PRIVATE SQUID_SHARED_EXPORTS
		)
	else()
		list(APPEND PROPERTIES  POSITION_INDEPENDENT_CODE True)
	endif()
	set_target_properties(${TARGET} PROPERTIES ${PROPERTIES})

	# Install the library
	install(TARGETS ${TARGET}
		EXPORT ${PROJECT_NAME}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

	# Install public header files
	foreach(HEADER ${P_PUBLIC_HEADERS})
		file(REAL_PATH ${HEADER} HEADER_REALPATH)
		file(RELATIVE_PATH HEADER_RELPATH ${SQUID_BASE_DIR} ${HEADER_REALPATH})
		get_filename_component(DIR ${HEADER_RELPATH} DIRECTORY)
		install(FILES ${HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${DIR})
	endforeach()
endfunction()

function(add_squid_library LIBNAME)
	if(SQUID_SHARED)
		PRIV_add_squid_library(${LIBNAME} SHARED ${ARGN})
	endif()
	
	if (SQUID_STATIC)
		PRIV_add_squid_library(${LIBNAME} STATIC ${ARGN})
	endif()
endfunction()

add_subdirectory(squid)

include(cmake/InstallBasicPackageFiles.cmake)

install_basic_package_files(${PROJECT_NAME}
    VERSION ${SQUID_VERSION}
    COMPATIBILITY SameMajorVersion
	DEPENDENCIES ${SQUID_BACKENDS}
)
