#!/bin/bash

THISDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cmake -P "${THISDIR}"/dist/dist.cmake
#cmake -DCOMMAND_ECHO=STDERR -P "${THISDIR}"/dist/dist.cmake
