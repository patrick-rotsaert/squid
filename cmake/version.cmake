#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

include_guard(GLOBAL)

math(EXPR ${PROJECT_NAME}_VERSION_NUMBER "(${${PROJECT_NAME}_VERSION_PATCH} + ${${PROJECT_NAME}_VERSION_MINOR} * 10000 + ${${PROJECT_NAME}_VERSION_MAJOR} * 10000 * 100)")
