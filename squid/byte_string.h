//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string>
#include <string_view>
#include <cstdint>

namespace squid {

using byte_string      = std::basic_string<std::uint8_t, std::char_traits<std::uint8_t>>;
using byte_string_view = std::basic_string_view<std::uint8_t, std::char_traits<std::uint8_t>>;

} // namespace squid
