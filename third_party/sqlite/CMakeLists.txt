#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)
#project(sqlite3 VERSION 3.36.0 LANGUAGES C)

set(TARGET sqlite3)

add_library(${TARGET} STATIC src/sqlite3.c src/sqlite3.h)

set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${TARGET} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(${TARGET} PROPERTIES C_STANDARD 99)

target_compile_definitions(${TARGET} PUBLIC
	SQLITE_OMIT_DEPRECATED=1
)

option(SQLITE_ENABLE_DBSTAT_VTAB    "enables dbstat virtual table"                    OFF)
option(SQLITE_ENABLE_FTS3           "enables full text searches version 3"            OFF)
option(SQLITE_ENABLE_FTS4           "enables full text searches version 3 & 4"        OFF)
option(SQLITE_ENABLE_FTS5           "enables full text searches version 5"            OFF)
option(SQLITE_ENABLE_GEOPOLY        "enables Geopoly extention"                       OFF)
option(SQLITE_ENABLE_ICU            "enables international components for unicode"    OFF)
option(SQLITE_ENABLE_JSON1          "enables JSON SQL functions"                      ON)
option(SQLITE_ENABLE_MATH_FUNCTIONS "enables the built-in SQL math functions"         ON)
option(SQLITE_ENABLE_RBU            "enables resumable bulk update extension"         OFF)
option(SQLITE_ENABLE_RTREE          "enables R*TRee index extension"                  OFF)
option(SQLITE_ENABLE_STAT4          "enhances query planner under certain situations" OFF)
option(SQLITE_USE_URI               "enables the default URI filename processing"     OFF)
option(SQLITE_RECOMMENDED_OPTIONS   "compile by SQLite3 recommended options"          ON)

target_compile_definitions(${TARGET} PRIVATE
	$<BUILD_INTERFACE:
		$<$<BOOL:${SQLITE_ENABLE_DBSTAT_VTAB}>:SQLITE_ENABLE_DBSTAT_VTAB>
		$<$<BOOL:${SQLITE_ENABLE_FTS3}>:SQLITE_ENABLE_FTS3>
		$<$<BOOL:${SQLITE_ENABLE_FTS4}>:SQLITE_ENABLE_FTS4>
		$<$<BOOL:${SQLITE_ENABLE_FTS5}>:SQLITE_ENABLE_FTS5>
		$<$<BOOL:${SQLITE_ENABLE_GEOPOLY}>:SQLITE_ENABLE_GEOPOLY>
		$<$<BOOL:${SQLITE_ENABLE_ICU}>:SQLITE_ENABLE_ICU>
		$<$<BOOL:${SQLITE_ENABLE_JSON1}>:SQLITE_ENABLE_JSON1>
		$<$<BOOL:${SQLITE_ENABLE_MATH_FUNCTIONS}>:SQLITE_ENABLE_MATH_FUNCTIONS>
		$<$<BOOL:${SQLITE_ENABLE_RBU}>:SQLITE_ENABLE_RBU>
		$<$<BOOL:${SQLITE_ENABLE_RTREE}>:SQLITE_ENABLE_RTREE>
		$<$<BOOL:${SQLITE_ENABLE_STAT4}>:SQLITE_ENABLE_STAT4>
		$<$<BOOL:${SQLITE_USE_URI}>:SQLITE_USE_URI>
		$<$<BOOL:${SQLITE_RECOMMENDED_OPTIONS}>:
			SQLITE_DQS=0
			SQLITE_DEFAULT_MEMSTATUS=0
			SQLITE_DEFAULT_WAL_SYNCHRONOUS=1
			SQLITE_LIKE_DOESNT_MATCH_BLOBS
			SQLITE_MAX_EXPR_DEPTH=0
			SQLITE_OMIT_DECLTYPE
			SQLITE_OMIT_DEPRECATED
			SQLITE_OMIT_PROGRESS_CALLBACK
			SQLITE_OMIT_SHARED_CACHE
			SQLITE_USE_ALLOCA
		>
	>
)

if (MSVC)
	target_compile_options(${TARGET} PRIVATE /wd4996)
	target_compile_definitions(${TARGET} PUBLIC SQLITE_APICALL=__stdcall)
endif()

add_library(SQLite::SQLite3 ALIAS ${TARGET})

if(${PROJECT_NAME_UC}_INSTALL)
	# Install the library
	install(TARGETS ${TARGET}
		EXPORT ${PROJECT_NAME}_Targets
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				COMPONENT ${COMPONENT_RUNTIME}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${COMPONENT_RUNTIME}
				NAMELINK_COMPONENT ${COMPONENT_DEVELOPMENT}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${COMPONENT_DEVELOPMENT}
	)

	# TODO: Install public header files
endif()
