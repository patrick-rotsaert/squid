#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if(NOT ${PROJECT_NAME_UC}_DEMOS)
	return()
endif()

add_subdirectory(demo_postgresql)
add_subdirectory(demo_sqlite3)
