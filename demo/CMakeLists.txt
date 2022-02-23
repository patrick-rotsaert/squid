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

# Add an interface library for squid_sqlite3 that demo apps can link to.
# It links the static (preferred) or shared version depending on which
# flavours are built.
add_library(lib_squid_sqlite3 INTERFACE)
if(SQUID_HAVE_SQLITE3)
	if(SQUID_STATIC)
		target_link_libraries(lib_squid_sqlite3 INTERFACE squid_sqlite3_static)
	else()
		target_link_libraries(lib_squid_sqlite3 INTERFACE squid_sqlite3)
	endif()
endif()

add_subdirectory(demo_postgresql)
add_subdirectory(demo_sqlite3)
