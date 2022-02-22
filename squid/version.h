//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

// Library version number
// Note: The value must be literal number, it is parsed by CMake.
// 0xMMmmpppp
//     MM = major
//     mm = minor
//   pppp = patch
#define SQUID_VERSION 0x00010000

#define SQUID_VERSION_MAJOR (SQUID_VERSION >> 24)
#define SQUID_VERSION_MINOR ((SQUID_VERSION >> 16) & 0xFF)
#define SQUID_VERSION_PATCH (SQUID_VERSION & 0xFFFF)
