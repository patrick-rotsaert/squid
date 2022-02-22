#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if(NOT SQUID_DEMOS)
	return()
endif()

# Add an interface library for squid_postgresql that demo apps can link to.
# It links the static (preferred) or shared version depending on which
# flavours are built.
add_library(lib_squid_postgresql INTERFACE)
if(SQUID_HAVE_POSTGRESQL)
	if(SQUID_STATIC)
		target_link_libraries(lib_squid_postgresql INTERFACE squid_postgresql_static)
	else()
		target_link_libraries(lib_squid_postgresql INTERFACE squid_postgresql)
	endif()
endif()

add_subdirectory(demo_postgresql)
