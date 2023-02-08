#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_guard(GLOBAL)

option(${PROJECT_NAME_UC}_TEST "Build the tests" OFF)
option(${PROJECT_NAME_UC}_RUN_UNIT_TESTS_ON_BUILD "Run the unit tests during build" OFF)

if(${PROJECT_NAME_UC}_TEST)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
	)

	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
endif()

if(${PROJECT_NAME_UC}_TEST AND ${PROJECT_NAME_UC}_RUN_UNIT_TESTS_ON_BUILD)
	function(run_unit_test_on_build TARGET)
		add_test(NAME ${TARGET} COMMAND ${TARGET})
		add_custom_command(TARGET ${TARGET} POST_BUILD
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "${TARGET}" --output-on-failure
		)
	endfunction()
else()
	function(run_unit_test_on_build TARGET)
	endfunction()
endif()

include(GoogleTest)
