//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/types.h"

#include <stdexcept>

namespace squid {
namespace postgresql {

void        hex_string_to_binary(std::string_view in, byte_string& out);
byte_string hex_string_to_binary(std::string_view in);

void        binary_to_hex_string(const unsigned char* begin, const unsigned char* end, std::string& out);
std::string binary_to_hex_string(const unsigned char* begin, const unsigned char* end);
void        binary_to_hex_string(byte_string_view in, std::string& out);
std::string binary_to_hex_string(byte_string_view in);

} // namespace postgresql
} // namespace squid
