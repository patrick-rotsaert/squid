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
			set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
			string(TOLOWER ${LSB_RELEASE_ID_SHORT} LSB_RELEASE_ID_SHORT_LC)
			execute_process(COMMAND ${DPKG} --print-architecture
				OUTPUT_VARIABLE DPKG_ARCH
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)
			set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
			# The default CPACK_PACKAGE_FILE_NAME is ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}
			# Overide the last bit so we have distro, release and architecture in the file name
			set(CPACK_SYSTEM_NAME "${LSB_RELEASE_ID_SHORT_LC}_${LSB_RELEASE_CODENAME_SHORT}_${DPKG_ARCH}")
		endif()
		# TODO: other distros
	endif()
elseif(WIN32)
	list(APPEND CPACK_GENERATOR ZIP)
	# TODO: NSIS, WIX (?)
endif()

include(CPack)
