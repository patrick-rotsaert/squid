#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SQUID - Another C++ Database Access Library")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BASE_DIR}/LICENSE")

set(GIT_URL)
find_program(GIT git)
if(GIT)
	execute_process(
		COMMAND git -C ${PROJECT_BASE_DIR} config --get remote.origin.url
		OUTPUT_VARIABLE GIT_URL
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()
if(GIT_URL)
	set(CPACK_PACKAGE_VENDOR "${GIT_URL}")
else()
	set(CPACK_PACKAGE_VENDOR "Patrick Rotsaert")
endif()
set(CPACK_PACKAGE_CONTACT "${CPACK_PACKAGE_VENDOR}")

set(CPACK_COMPONENTS_ALL runtime development)

set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development files")

if(UNIX)
	list(APPEND CPACK_GENERATOR TGZ)

	find_program(LSB_RELEASE lsb_release)
	if(LSB_RELEASE)
		execute_process(COMMAND ${LSB_RELEASE} -is
			OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		execute_process(COMMAND lsb_release -cs
			OUTPUT_VARIABLE LSB_RELEASE_CODENAME_SHORT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		find_program(DPKG dpkg)
		if(DPKG)
			# Assume debian based distro
			list(APPEND CPACK_GENERATOR DEB)

			# Get architecture name
			string(TOLOWER ${LSB_RELEASE_ID_SHORT} LSB_RELEASE_ID_SHORT_LC)
			execute_process(COMMAND ${DPKG} --print-architecture
				OUTPUT_VARIABLE DPKG_ARCH
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)

			# The default CPACK_PACKAGE_FILE_NAME is ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}
			# Overide the last bit so we have distro, release and architecture in the file name
			set(CPACK_SYSTEM_NAME "${LSB_RELEASE_ID_SHORT_LC}_${LSB_RELEASE_CODENAME_SHORT}_${DPKG_ARCH}")

			set(CPACK_DEB_COMPONENT_INSTALL ON)

			# Set the package names for runtime and dev
			set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
			set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-dev)

			# The development package depends on the runtime and database dev packages (for header files)
			# Note the separator ", "
			set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_DEPENDS "${CPACK_PACKAGE_NAME}, libpq-dev, libsqlite3-dev")

			# Set the package desriptions for runtime and dev
			set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} (${CPACK_COMPONENT_RUNTIME_DISPLAY_NAME})")
			set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} (${CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME})")

			# Detect and include dependencies
			set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
		endif()
		# TODO: other distros
	endif()
elseif(WIN32)
	list(APPEND CPACK_GENERATOR ZIP)
	# TODO: NSIS, WIX (?)
endif()

include(CPack)
