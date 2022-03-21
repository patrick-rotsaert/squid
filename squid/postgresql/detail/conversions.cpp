//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/detail/conversions.h"

namespace squid {
namespace postgresql {

namespace {

byte_string::value_type hex_char_to_nibble(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if (c >= 'a' && c <= 'f')
	{
		return 0xa + c - 'a';
	}
	else if (c >= 'A' && c <= 'F')
	{
		return 0xa + c - 'A';
	}
	else
	{
		throw std::runtime_error{ "illegal hex character" };
	}
}

} // namespace

void hex_string_to_binary(std::string_view in, byte_string& out)
{
	if (!in.starts_with("\\x"))
	{
		throw std::runtime_error{ "string does not start with \"\\x\"" };
	}

	if (in.length() % 2)
	{
		throw std::runtime_error{ "string length is not a multiple of 2" };
	}

	out.resize((in.length() - 2) / 2);
	auto p = out.data();

	for (auto it = in.begin() + 2, end = in.end(); it != end;)
	{
		*p = (hex_char_to_nibble(*it++) << 4);
		*p++ |= hex_char_to_nibble(*it++);
	}
}

byte_string hex_string_to_binary(std::string_view in)
{
	byte_string result;
	hex_string_to_binary(in, result);
	return result;
}

void binary_to_hex_string(const unsigned char* begin, const unsigned char* end, std::string& out)
{
	constexpr auto hex = "0123456789ABCDEF";

	out.resize(2 + 2 * std::distance(begin, end));
	auto p = out.data();
	*p++   = '\\';
	*p++   = 'x';
	for (auto it = begin; it < end; ++it)
	{
		*p++ = hex[*it >> 4];
		*p++ = hex[*it & 15];
	}
}

std::string binary_to_hex_string(const unsigned char* begin, const unsigned char* end)
{
	std::string result;
	binary_to_hex_string(begin, end, result);
	return result;
}

void binary_to_hex_string(byte_string_view in, std::string& out)
{
	binary_to_hex_string(&(*in.begin()), &(*in.end()), out);
}

std::string binary_to_hex_string(byte_string_view in)
{
	std::string result;
	binary_to_hex_string(in, result);
	return result;
}

} // namespace postgresql
} // namespace squid
