//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "squid/detail/conversions.h"

#include <regex>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>

#if defined(_MSC_VER)
#pragma warning(disable : 4456)
#endif

namespace squid {

void string_to_time_point(std::string_view in, time_point& out)
{
	// TODO: Use std::chrono::parse when it becomes available in libstdc++

	// https://github.com/HowardHinnant/date.git would be a great alternative,
	// but I prefer not to have any additional dependencies other than the database client libraries.

	static const std::regex re{ R"(^(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})(\.\d*)?( ?([+-]\d{2})(:?(\d{2}))?)?$)" };
	//                              1       2       3       4       5       6      7       8  9          10 11

	std::match_results<std::string_view::const_iterator> matches;
	if (!std::regex_match(in.begin(), in.end(), matches, re) || matches.size() != 1 + 11)
	{
		throw std::invalid_argument{ "invalid time point format" };
	}

	const auto year    = string_to_number<int>(matches[1].str());
	const auto month   = string_to_number<unsigned>(matches[2].str());
	const auto day     = string_to_number<unsigned>(matches[3].str());
	const auto hours   = string_to_number<unsigned>(matches[4].str());
	const auto minutes = string_to_number<unsigned>(matches[5].str());
	const auto seconds = string_to_number<unsigned>(matches[6].str());

	out = std::chrono::sys_days{ std::chrono::year{ year } / std::chrono::month{ month } / day } + std::chrono::hours{ hours } +
	      std::chrono::minutes{ minutes } + std::chrono::seconds{ seconds };

	if (matches.length(7) > 1)
	{
		const auto microseconds = std::lround(string_to_number<double>(matches[7].str()) * 1e6);
		out += std::chrono::microseconds{ microseconds };
	}

	if (matches.length(9))
	{
		const auto hours            = matches[9].str();
		const auto utc_offset_hours = string_to_number<int>((hours.front() == '+') ? &hours[1] : hours);
		out -= std::chrono::hours{ utc_offset_hours };

		if (matches.length(11))
		{
			const auto utc_offset_minutes = string_to_number<int>(matches[11].str()) * (utc_offset_hours < 0 ? -1 : 1);
			out -= std::chrono::minutes{ utc_offset_minutes };
		}
	}
}

time_point string_to_time_point(std::string_view in)
{
	time_point result{};
	string_to_time_point(in, result);
	return result;
}

void string_to_date(std::string_view in, date& out)
{
	// TODO: Use std::chrono::parse when it becomes available in libstdc++

	// https://github.com/HowardHinnant/date.git would be a great alternative,
	// but I prefer not to have any additional dependencies other than the database client libraries.

	static const std::regex re{ R"(^(\d{4})-(\d{2})-(\d{2})$)" };
	//                              1       2       3

	std::match_results<std::string_view::const_iterator> matches;
	if (!std::regex_match(in.begin(), in.end(), matches, re) || matches.size() != 1 + 3)
	{
		throw std::invalid_argument{ "invalid date format" };
	}

	const auto year  = string_to_number<int>(matches[1].str());
	const auto month = string_to_number<unsigned>(matches[2].str());
	const auto day   = string_to_number<unsigned>(matches[3].str());

	out = std::chrono::sys_days{ std::chrono::year{ year } / std::chrono::month{ month } / day };
}

date string_to_date(std::string_view in)
{
	date result{};
	string_to_date(in, result);
	return result;
}

void string_to_time_of_day(std::string_view in, time_of_day& out)
{
	// TODO: Use std::chrono::parse when it becomes available in libstdc++

	// https://github.com/HowardHinnant/date.git would be a great alternative,
	// but I prefer not to have any additional dependencies other than the database client libraries.

	static const std::regex re{ R"(^(\d{2}):(\d{2}):(\d{2})(\.\d*)?( ?([+-]\d{2})(:?(\d{2}))?)?$)" };
	//                              1       2       3      4       5  6          7  8

	std::match_results<std::string_view::const_iterator> matches;
	if (!std::regex_match(in.begin(), in.end(), matches, re) || matches.size() != 1 + 8)
	{
		throw std::invalid_argument{ "invalid time of day format" };
	}

	const auto hours   = string_to_number<unsigned>(matches[1].str());
	const auto minutes = string_to_number<unsigned>(matches[2].str());
	const auto seconds = string_to_number<unsigned>(matches[3].str());

	std::chrono::microseconds tmp{ (3600LL * hours + 60 * minutes + seconds) * 1000000LL };

	if (matches.length(4) > 1)
	{
		const auto microseconds = std::lround(string_to_number<double>(matches[4].str()) * 1e6);
		tmp += std::chrono::microseconds{ microseconds };
	}

	if (matches.length(6))
	{
		const auto hours            = matches[6].str();
		const auto utc_offset_hours = string_to_number<int>((hours.front() == '+') ? &hours[1] : hours);
		tmp -= std::chrono::hours{ utc_offset_hours };

		if (matches.length(8))
		{
			const auto utc_offset_minutes = string_to_number<int>(matches[8].str()) * (utc_offset_hours < 0 ? -1 : 1);
			tmp -= std::chrono::minutes{ utc_offset_minutes };
		}
	}

	out = time_of_day{ tmp };
}

time_of_day string_to_time_of_day(std::string_view in)
{
	time_of_day result{};
	string_to_time_of_day(in, result);
	return result;
}

void time_point_to_string(const time_point& in, std::string& out)
{
	// TODO: use std::format when it becomes available in libstdc++
	using namespace std::chrono;
	auto           dp = floor<days>(in);
	year_month_day date{ dp };
	hh_mm_ss       time{ floor<microseconds>(in - dp) };

	if (time.subseconds().count())
	{
		out.resize(10 * 3 + 19 * 4 + 6 + 1);
		std::sprintf(out.data(),
		             "%04d-%02u-%02u %02ld:%02ld:%02ld.%06ld",
		             static_cast<int>(date.year()),
		             static_cast<unsigned>(date.month()),
		             static_cast<unsigned>(date.day()),
		             static_cast<long>(time.hours().count()),
		             static_cast<long>(time.minutes().count()),
		             static_cast<long>(time.seconds().count()),
		             static_cast<long>(time.subseconds().count()));
	}
	else
	{
		out.resize(10 * 3 + 19 * 3 + 5 + 1);
		std::sprintf(out.data(),
		             "%04d-%02u-%02u %02ld:%02ld:%02ld",
		             static_cast<int>(date.year()),
		             static_cast<unsigned>(date.month()),
		             static_cast<unsigned>(date.day()),
		             static_cast<long>(time.hours().count()),
		             static_cast<long>(time.minutes().count()),
		             static_cast<long>(time.seconds().count()));
	}
	out.resize(std::strlen(out.data()));
}

std::string time_point_to_string(const time_point& in)
{
	std::string result;
	time_point_to_string(in, result);
	return result;
}

void date_to_string(const date& in, std::string& out)
{
	// TODO: use std::format when it becomes available in libstdc++
	out.resize(10 * 3 + 3 + 1);
	std::sprintf(
	    out.data(), "%04d-%02u-%02u", static_cast<int>(in.year()), static_cast<unsigned>(in.month()), static_cast<unsigned>(in.day()));
	out.resize(std::strlen(out.data()));
}

std::string date_to_string(const date& in)
{
	std::string result;
	date_to_string(in, result);
	return result;
}

void time_of_day_to_string(const time_of_day& in, std::string& out)
{
	// TODO: use std::format when it becomes available in libstdc++
	if (in.subseconds().count())
	{
		out.resize(19 * 4 + 3 + 1);
		std::sprintf(out.data(),
		             "%02ld:%02ld:%02ld.%06ld",
		             static_cast<long>(in.hours().count()),
		             static_cast<long>(in.minutes().count()),
		             static_cast<long>(in.seconds().count()),
		             static_cast<long>(in.subseconds().count()));
	}
	else
	{
		out.resize(19 * 3 + 2 + 1);
		std::sprintf(out.data(),
		             "%02ld:%02ld:%02ld",
		             static_cast<long>(in.hours().count()),
		             static_cast<long>(in.minutes().count()),
		             static_cast<long>(in.seconds().count()));
	}
	out.resize(std::strlen(out.data()));
}

std::string time_of_day_to_string(const time_of_day& in)
{
	std::string result;
	time_of_day_to_string(in, result);
	return result;
}

} // namespace squid
