#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)
#project(sqlite3 VERSION 3.36.0 LANGUAGES C)

set(TARGET sqlite3)

add_library(${TARGET} src/sqlite3.c src/sqlite3.h)

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

# option(BUILD_ENABLE_DBSTAT_VTAB "enables dbstat virtual table"                 OFF)
# option(BUILD_ENABLE_FTS3        "enables full text searches version 3"         OFF)
# option(BUILD_ENABLE_FTS5        "enables full text searches version 5"         OFF)
# option(BUILD_ENABLE_ICU         "enables international components for unicode" OFF)
# option(BUILD_ENABLE_JSON1       "enables JSON SQL functions"                   OFF)
# option(BUILD_ENABLE_RBU         "enables resumable bulk update extension"      OFF)
# option(BUILD_ENABLE_RTREE       "enables R*TRee index extension"               OFF)

# if(BUILD_ENABLE_ICU)
# 	target_compile_definitions(${TARGET} PRIVATE SQLITE_ENABLE_ICU=1)
# endif()
# if(BUILD_ENABLE_RBU)
# 	target_compile_definitions(${TARGET} PRIVATE SQLITE_ENABLE_RBU=1)
# endif()
# if(BUILD_ENABLE_FTS3)
# 	target_compile_definitions(${TARGET} PRIVATE SQLITE_ENABLE_FTS3=1)
# endif()
# if(BUILD_ENABLE_FTS5)
# 	target_compile_definitions(${TARGET} PRIVATE SQLITE_ENABLE_FTS5=1)
# endif()
# if(BUILD_ENABLE_JSON1)
# 	target_compile_definitions(${TARGET} PRIVATE SQLITE_ENABLE_JSON1=1)
# endif()
# if(BUILD_ENABLE_RTREE)
# 	target_compile_definitions(${TARGET} PRIVATE SQLITE_ENABLE_RTREE=1)
# endif()
# if(BUILD_ENABLE_DBSTAT_VTAB)
# 	target_compile_definitions(${TARGET} PRIVATE SQLITE_ENABLE_DBSTAT_VTAB=1)
# endif()

if (MSVC)
	target_compile_options(${TARGET} PRIVATE /wd4996)
	target_compile_definitions(${TARGET} PUBLIC SQLITE_APICALL=__stdcall)
	 if(BUILD_SHARED_LIBS)		
	 	target_compile_definitions(${TARGET}
			PRIVATE
				SQLITE_API=__declspec\(dllexport\)
	 		INTERFACE
				SQLITE_API=__declspec\(dllimport\)
	 	)
	 endif()
else()
	if(BUILD_SHARED_LIBS)		
		target_compile_definitions(${TARGET} PUBLIC SQLITE_API=__attribute__\(\(visibility\(\"default\"\)\)\))
	endif()
endif()

if(UNIX AND NOT APPLE)
	target_link_libraries(${TARGET} PUBLIC dl)
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

	# Install public header files
endif()

# TODO

# # Installation
# set(SQLITE3_INSTALL_BIN_DIR bin)
# set(SQLITE3_INSTALL_INC_DIR include/sqlite)
# set(SQLITE3_INSTALL_LIB_DIR lib)
# set(SQLITE3_INSTALL_CMAKE_DIR cmake)

# set(SQLITE3_SHARED_TARGETS_FILE ${PROJECT_NAME}-shared-targets)
# set(SQLITE3_STATIC_TARGETS_FILE ${PROJECT_NAME}-static-targets)

# if(BUILD_SHARED_LIBS)
# 	set(SQLITE3_INSTALL_TARGET_EXPORT_FILE ${SQLITE3_SHARED_TARGETS_FILE})
# else()
# 	set(SQLITE3_INSTALL_TARGET_EXPORT_FILE ${SQLITE3_STATIC_TARGETS_FILE})
# endif()

# install(FILES src/sqlite3.h COMPONENT ${PROJECT_NAME} DESTINATION ${SQLITE3_INSTALL_INC_DIR})
# install(TARGETS ${PROJECT_NAME} EXPORT ${SQLITE3_INSTALL_TARGET_EXPORT_FILE} COMPONENT ${PROJECT_NAME}
# 	RUNTIME DESTINATION ${SQLITE3_INSTALL_BIN_DIR} # Windows: DLL, EXE
# 	LIBRARY DESTINATION ${SQLITE3_INSTALL_BIN_DIR} # Linux/Mac: .so, .dylib
# 	ARCHIVE DESTINATION ${SQLITE3_INSTALL_LIB_DIR} # Static libs / import libs
# )

# dependency_install_pdb(${PROJECT_NAME})

# install(EXPORT ${SQLITE3_INSTALL_TARGET_EXPORT_FILE} COMPONENT ${PROJECT_NAME} NAMESPACE SQLite:: DESTINATION ${SQLITE3_INSTALL_CMAKE_DIR})

# include(CMakePackageConfigHelpers)

# set(SQLITE3_CMAKE_SHARED_TARGETS_FILE ${SQLITE3_INSTALL_CMAKE_DIR}/${SQLITE3_SHARED_TARGETS_FILE}.cmake)
# set(SQLITE3_CMAKE_STATIC_TARGETS_FILE ${SQLITE3_INSTALL_CMAKE_DIR}/${SQLITE3_STATIC_TARGETS_FILE}.cmake)

# configure_package_config_file(
# 	${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake
# 	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
# 	INSTALL_DESTINATION ${SQLITE3_INSTALL_CMAKE_DIR}
# 	PATH_VARS SQLITE3_CMAKE_SHARED_TARGETS_FILE SQLITE3_CMAKE_STATIC_TARGETS_FILE
# 	NO_CHECK_REQUIRED_COMPONENTS_MACRO
# )
# dependency_write_basic_package_version_file(
# 	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake 
# 	${PROJECT_VERSION}
# )
# install(
# 	FILES
# 		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
# 		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
# 	COMPONENT ${PROJECT_NAME}
# 	DESTINATION ${SQLITE3_INSTALL_CMAKE_DIR}
# )
