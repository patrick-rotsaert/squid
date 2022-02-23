#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.17)

if(NOT SQUID_HAVE_SQLITE3)
	return()
endif()

add_squid_library(squid_sqlite3
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

	PUBLIC_SQUID_LIBRARIES
		squid
)
