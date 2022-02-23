#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if(NOT SQUID_HAVE_SQLITE3)
	return()
endif()

set(TARGET demo_sqlite3)
add_executable(${TARGET} demo_sqlite3.cpp)

target_compile_features(${TARGET} PRIVATE cxx_std_20)
target_link_libraries(${TARGET} PRIVATE lib_squid_sqlite3)
