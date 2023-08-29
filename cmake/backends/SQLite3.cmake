#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_guard(GLOBAL)

# On WIN32, prefer embedded SQLite.
option(${OPTION_NAME}_EMBEDDED "Enable embedded ${BACKEND_NAME} backend" ${WIN32})
if(${OPTION_NAME}_EMBEDDED)
	add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/sqlite ${CMAKE_BINARY_DIR}/third_party/sqlite)
	set(${BACKEND_NAME}_FOUND TRUE)
	return()
endif()

set(SQLITE3_FIND_QUIETLY TRUE)
find_package(SQLite3 QUIET)
