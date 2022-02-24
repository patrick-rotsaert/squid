#
# Copyright (C) 2022 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.17)

add_squid_library(squid
	SOURCES
		parameter.cpp
		result.cpp
		error.cpp
		connection.cpp
		connectionpool.cpp
		basicstatement.cpp
		statement.cpp
		preparedstatement.cpp
		transaction.cpp

		detail/always_false.h
		detail/conversions.cpp
		detail/conversions.h
		detail/demangle.cpp
		detail/demangled_type_name.h
		detail/demangle.h
		detail/is_optional.h
		detail/is_scoped_enum.h

	PUBLIC_HEADERS
		api.h
		parameter.h
		result.h
		error.h
		ibackendstatement.h
		ibackendconnection.h
		ibackendconnectionfactory.h
		connection.h
		connectionpool.h
		basicstatement.h
		statement.h
		preparedstatement.h
		transaction.h
)

add_subdirectory(postgresql)
add_subdirectory(sqlite3)
