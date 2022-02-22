#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.17)

if(NOT PostgreSQL_FOUND)
	return()
endif()

add_squid_library(squid_postgresql
	SOURCES
		postgresqlerror.cpp
		basicpostgresqlstatement.cpp
		postgresqlstatement.cpp
		postgresqlpreparedstatement.cpp
		postgresqlbackendconnection.cpp
		postgresqlbackendconnectionfactory.cpp
		postgresqlconnection.cpp

		detail/pgconversions.cpp
		detail/pgconversions.h
		detail/postgresqlconnectionchecker.cpp
		detail/postgresqlconnectionchecker.h
		detail/postgresqlquery.cpp
		detail/postgresqlquery.h
		detail/postgresqlqueryparameters.cpp
		detail/postgresqlqueryparameters.h
		detail/postgresqlqueryresults.cpp
		detail/postgresqlqueryresults.h
		
	PUBLIC_HEADERS
		postgresqlerror.h
		basicpostgresqlstatement.h
		postgresqlstatement.h
		postgresqlpreparedstatement.h
		postgresqlbackendconnection.h
		postgresqlbackendconnectionfactory.h
		postgresqlconnection.h

	PUBLIC_LIBRARIES
		PostgreSQL::PostgreSQL

	PUBLIC_SQUID_LIBRARIES
		squid
)
