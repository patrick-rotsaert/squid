#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)

if(NOT ${PROJECT_NAME}_HAVE_SQLITE3)
	return()
endif()

add_project_library(sqlite3
	SOURCES
		error.cpp
		statement.cpp
		backendconnection.cpp
		backendconnectionfactory.cpp
		connection.cpp
		
	PUBLIC_HEADERS
		error.h
		statement.h
		backendconnection.h
		backendconnectionfactory.h
		connection.h

	PUBLIC_LIBRARIES
		SQLite::SQLite3
		squid::common
)
