#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/paths.cmake)

file(
	STRINGS ${SQUID_BASE_DIR}/squid/version.h _VERSION_NUMBER
	REGEX "#define SQUID_VERSION (0x[0-9a-fA-F]+)"
)
string(REGEX MATCH "(0x[0-9a-fA-F]+)" _VERSION_NUMBER "${_VERSION_NUMBER}")

if(NOT _VERSION_NUMBER)
	message(FATAL_ERROR "Could not get version number from ${SQUID_BASE_DIR}/squid/version.h")
endif()

math(EXPR SQUID_VERSION_MAJOR "${_VERSION_NUMBER} >> 24")
math(EXPR SQUID_VERSION_MINOR "(${_VERSION_NUMBER} >> 16) & 0xFF")
math(EXPR SQUID_VERSION_PATCH "${_VERSION_NUMBER} & 0xFFFF")

set(SQUID_VERSION ${SQUID_VERSION_MAJOR}.${SQUID_VERSION_MINOR}.${SQUID_VERSION_PATCH})
set(SQUID_SOVERSION ${SQUID_VERSION_MAJOR}.${SQUID_VERSION_MINOR})
