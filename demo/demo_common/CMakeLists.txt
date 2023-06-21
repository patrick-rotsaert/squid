#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)

add_project_library(demo_common
	SOURCES
		demo_common.cpp
		demo_common.h
	PUBLIC_LIBRARIES
		squid::common
)
