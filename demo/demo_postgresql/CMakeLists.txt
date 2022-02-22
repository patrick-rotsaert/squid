#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if(NOT SQUID_HAVE_POSTGRESQL)
	return()
endif()

set(TARGET demo_postgresql)
add_executable(${TARGET} demo_postgresql.cpp)

target_compile_features(${TARGET} PRIVATE cxx_std_20)
target_link_libraries(${TARGET} PRIVATE lib_squid_postgresql)
