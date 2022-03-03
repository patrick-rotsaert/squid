//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <chrono>

namespace squid {

/// Type to be used to bind binary strings, both for parameters and results.
using byte_string = std::basic_string<std::uint8_t, std::char_traits<std::uint8_t>>;

/// Type to be used to bind binary strings, only for parameters.
using byte_string_view = std::basic_string_view<std::uint8_t, std::char_traits<std::uint8_t>>;

using time_point  = std::chrono::system_clock::time_point;
using date        = std::chrono::year_month_day;
using time_of_day = std::chrono::hh_mm_ss<std::chrono::microseconds>;

} // namespace squid
