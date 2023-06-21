#!/bin/bash

#
# Copyright (C) 2022-2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

THISDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# TODO: support command line options to:
#  - specify the build directory

cmake -P "${THISDIR}"/dist/dist.cmake
#cmake -DCOMMAND_ECHO=STDERR -P "${THISDIR}"/dist/dist.cmake
