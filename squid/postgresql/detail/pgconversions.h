//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/byte_string.h"

#include <stdexcept>

namespace squid {
namespace postgresql {

void        pg_hex_string_to_binary(std::string_view in, byte_string& out);
byte_string pg_hex_string_to_binary(std::string_view in);

void        binary_to_pg_hex_string(const unsigned char* begin, const unsigned char* end, std::string& out);
std::string binary_to_pg_hex_string(const unsigned char* begin, const unsigned char* end);

template<class It>
inline void binary_to_pg_hex_string(const It begin, const It end, std::string& out)
{
	binary_to_pg_hex_string(&(*begin), &(*end), out);
}

template<class It>
inline std::string binary_to_pg_hex_string(const It begin, const It end)
{
	return binary_to_pg_hex_string(&(*begin), &(*end));
}

} // namespace postgresql
} // namespace squid
