//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/types.h"
#include "squid/config.h"

#ifdef SQUID_HAVE_BOOST_DATE_TIME
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#endif

#include <type_traits>
#include <charconv>
#include <stdexcept>
#include <system_error>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string_view>

namespace squid {

template<typename T, class = typename std::enable_if<std::is_scalar_v<T>>::type>
inline void string_to_number(const char* first, const char* last, T& out)
{
	std::from_chars_result res = std::from_chars(first, last, out);
	if (res.ec != std::errc{})
	{
		throw std::system_error{ std::error_code(static_cast<int>(res.ec), std::generic_category()) };
	}
	else if (res.ptr != last)
	{
		std::ostringstream msg;
		msg << "conversion incomplete, remaining input is " << std::quoted(std::string_view{ res.ptr, last });
		throw std::invalid_argument{ msg.str() };
	}
}

template<typename T, class = typename std::enable_if<std::is_scalar_v<T>>::type>
inline void string_to_number(std::string_view in, T& out)
{
	string_to_number(in.data(), in.data() + in.length(), out);
}

template<typename T, class = typename std::enable_if<std::is_scalar_v<T>>::type>
inline T string_to_number(std::string_view in)
{
	T out{};
	string_to_number(in.data(), in.data() + in.length(), out);
	return out;
}

void SQUID_EXPORT       string_to_time_point(std::string_view in, time_point& out);
time_point SQUID_EXPORT string_to_time_point(std::string_view in);

void SQUID_EXPORT string_to_date(std::string_view in, date& out);
date SQUID_EXPORT string_to_date(std::string_view in);

void SQUID_EXPORT        string_to_time_of_day(std::string_view in, time_of_day& out);
time_of_day SQUID_EXPORT string_to_time_of_day(std::string_view in);

void SQUID_EXPORT        time_point_to_string(const time_point& in, std::string& out);
std::string SQUID_EXPORT time_point_to_string(const time_point& in);

void SQUID_EXPORT        date_to_string(const date& in, std::string& out);
std::string SQUID_EXPORT date_to_string(const date& in);

void SQUID_EXPORT        time_of_day_to_string(const time_of_day& in, std::string& out);
std::string SQUID_EXPORT time_of_day_to_string(const time_of_day& in);

#ifdef SQUID_HAVE_BOOST_DATE_TIME
void SQUID_EXPORT                     string_to_boost_ptime(std::string_view in, boost::posix_time::ptime& out);
boost::posix_time::ptime SQUID_EXPORT string_to_boost_ptime(std::string_view in);

void SQUID_EXPORT        boost_ptime_to_string(const boost::posix_time::ptime& in, std::string& out);
std::string SQUID_EXPORT boost_ptime_to_string(const boost::posix_time::ptime& in);

void SQUID_EXPORT                   string_to_boost_date(std::string_view in, boost::gregorian::date& out);
boost::gregorian::date SQUID_EXPORT string_to_boost_date(std::string_view in);

void SQUID_EXPORT        boost_date_to_string(const boost::gregorian::date& in, std::string& out);
std::string SQUID_EXPORT boost_date_to_string(const boost::gregorian::date& in);

void SQUID_EXPORT                             string_to_boost_time_duration(std::string_view in, boost::posix_time::time_duration& out);
boost::posix_time::time_duration SQUID_EXPORT string_to_boost_time_duration(std::string_view in);

void SQUID_EXPORT        boost_time_duration_to_string(const boost::posix_time::time_duration& in, std::string& out);
std::string SQUID_EXPORT boost_time_duration_to_string(const boost::posix_time::time_duration& in);
#endif

} // namespace squid
