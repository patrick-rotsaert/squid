#!/bin/bash

THISDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# TODO: support command line options to:
#  - specify the build directory

cmake -P "${THISDIR}"/dist/dist.cmake
#cmake -DCOMMAND_ECHO=STDERR -P "${THISDIR}"/dist/dist.cmake
