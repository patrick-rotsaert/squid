#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.17)

if(NOT ${PROJECT_NAME}_HAVE_POSTGRESQL)
	return()
endif()

add_project_library(postgresql
	SOURCES
		error.cpp
		basicstatement.cpp
		statement.cpp
		preparedstatement.cpp
		backendconnection.cpp
		backendconnectionfactory.cpp
		connection.cpp

		detail/conversions.cpp
		detail/conversions.h
		detail/connectionchecker.cpp
		detail/connectionchecker.h
		detail/query.cpp
		detail/query.h
		detail/queryparameters.cpp
		detail/queryparameters.h
		detail/queryresults.cpp
		detail/queryresults.h
		
	PUBLIC_HEADERS
		error.h
		basicstatement.h
		statement.h
		preparedstatement.h
		backendconnection.h
		backendconnectionfactory.h
		connection.h

	PUBLIC_LIBRARIES
		PostgreSQL::PostgreSQL
		squid::common
)
