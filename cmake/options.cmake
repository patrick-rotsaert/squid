#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_guard(GLOBAL)

option(SQUID_SHARED "Enable build of shared libraries" ON)
option(SQUID_STATIC "Enable build of static libraries" ON)

if((NOT SQUID_SHARED) AND (NOT SQUID_STATIC))
	message(FATAL_ERROR "Options SQUID_SHARED and SQUID_STATIC cannot both be OFF")
endif()

option(SQUID_DEMOS "Build the demo apps" OFF)
