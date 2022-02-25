//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"

#include <type_traits>
#include <charconv>
#include <stdexcept>
#include <system_error>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string_view>

namespace squid {

template<typename T, class = typename std::enable_if<std::is_scalar<T>::value>::type>
inline void string_to_number(const char* first, const char* last, T& out)
{
	std::from_chars_result res = std::from_chars(first, last, out);
	if (res.ec != std::errc())
	{
		throw std::system_error{ std::error_code(static_cast<int>(res.ec), std::generic_category()) };
	}
	else if (res.ptr != last)
	{
		std::ostringstream error;
		error << "conversion incomplete, remaining input is " << std::quoted(std::string_view{ res.ptr, last });
		throw std::invalid_argument{ error.str() };
	}
}

template<typename T, class = typename std::enable_if<std::is_scalar<T>::value>::type>
inline void string_to_number(std::string_view in, T& out)
{
	string_to_number(in.begin(), in.end(), out);
}

template<typename T, class = typename std::enable_if<std::is_scalar<T>::value>::type>
inline T string_to_number(std::string_view in)
{
	T out{};
	string_to_number(in.begin(), in.end(), out);
	return out;
}

void SQUID_EXPORT                                  string_to_time_point(std::string_view in, std::chrono::system_clock::time_point& out);
std::chrono::system_clock::time_point SQUID_EXPORT string_to_time_point(std::string_view in);

void SQUID_EXPORT                        string_to_year_month_day(std::string_view in, std::chrono::year_month_day& out);
std::chrono::year_month_day SQUID_EXPORT string_to_year_month_day(std::string_view in);

void SQUID_EXPORT string_to_hh_mm_ss(std::string_view in, std::chrono::hh_mm_ss<std::chrono::microseconds>& out);
std::chrono::hh_mm_ss<std::chrono::microseconds> SQUID_EXPORT string_to_hh_mm_ss(std::string_view in);

void SQUID_EXPORT        time_point_to_string(const std::chrono::system_clock::time_point& in, std::string& out);
std::string SQUID_EXPORT time_point_to_string(const std::chrono::system_clock::time_point& in);

void SQUID_EXPORT        year_month_day_to_string(const std::chrono::year_month_day& in, std::string& out);
std::string SQUID_EXPORT year_month_day_to_string(const std::chrono::year_month_day& in);

void SQUID_EXPORT        hh_mm_ss_to_string(const std::chrono::hh_mm_ss<std::chrono::microseconds>& in, std::string& out);
std::string SQUID_EXPORT hh_mm_ss_to_string(const std::chrono::hh_mm_ss<std::chrono::microseconds>& in);

} // namespace squid
