#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)

add_project_library(common
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
		version.cpp

		detail/always_false.h
		detail/conversions.cpp
		detail/conversions.h
		detail/demangle.cpp
		detail/demangled_type_name.h
		detail/demangle.h

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
		types.h

		detail/is_optional.h
		detail/is_scoped_enum.h
		detail/parameterbinder.h
		detail/resultbinder.h
		detail/type_traits.h
		detail/bind_oarchive.h
		detail/bind_iarchive.h

	UNIT_TEST_SOURCES
		test/unit/test_parameter.cpp
		test/unit/test_result.cpp
		test/unit/test_conversions.cpp

	PUBLIC_INCLUDE_DIRS
		${CMAKE_BINARY_DIR} # for configured headers, see below
)

find_package(Boost COMPONENTS serialization)
if(Boost_SERIALIZATION_FOUND)
	set(SQUID_HAVE_BOOST_SERIALIZATION 1)
	target_link_libraries(common PUBLIC Boost::serialization)
endif()

configure_file(
	version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/version.h
)

configure_file(
	config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
)

if(${PROJECT_NAME_UC}_INSTALL)
	install_project_header(${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_BINARY_DIR})
	install_project_header(${CMAKE_CURRENT_BINARY_DIR}/config.h ${CMAKE_BINARY_DIR})
endif()

add_subdirectory(postgresql)
add_subdirectory(sqlite3)
