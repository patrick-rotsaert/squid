#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)

if(NOT ${PROJECT_NAME}_HAVE_SQLITE3)
	return()
endif()

add_project_library(sqlite
	SOURCES
		error.cpp
		statement.cpp
		backendconnection.cpp
		backendconnectionfactory.cpp
		connection.cpp

		detail/queryparameters.cpp
		detail/queryparameters.h
		detail/queryresults.cpp
		detail/queryresults.h
		detail/sqlite3fwd.h
		detail/isqliteapi.cpp
		detail/isqliteapi.h
		detail/sqliteapi.cpp
		detail/sqliteapi.h

	UNIT_TEST_SOURCES
		test/unit/test_error.cpp
		test/unit/test_backendconnection.cpp
		test/unit/test_backendconnectionfactory.cpp
		test/unit/test_connection.cpp

		detail/test/unit/test_queryparameters.cpp
		detail/test/unit/test_queryresults.cpp

	MOCK_SOURCES
		detail/sqliteapimock.cpp
		detail/sqliteapimock.h

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
