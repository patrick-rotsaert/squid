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
#include "squid/config.h"

#ifdef SQUID_HAVE_BOOST_DATE_TIME
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#endif

#include <optional>
#include <regex>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>

#if defined(_MSC_VER)
#pragma warning(disable : 4456)
#endif

namespace squid {

namespace {

struct parsed_time_point
{
	int                   year;
	unsigned              month;
	unsigned              day;
	unsigned              hours;
	unsigned              minutes;
	unsigned              seconds;
	std::optional<double> fracseconds;
	std::optional<int>    utc_offset_minutes;
};

parsed_time_point parse_time_point(std::string_view in)
{
	parsed_time_point out{};

	static const std::regex re{ R"(^(\d{4})-(\d{2})-(\d{2})[T ](\d{2}):(\d{2}):(\d{2})(\.\d*)?( ?([+-]\d{2})(:?(\d{2}))?|Z)?$)" };
	//                              1       2       3          4       5       6      7       8  9          10 11

	std::match_results<std::string_view::const_iterator> matches;
	if (!std::regex_match(in.begin(), in.end(), matches, re) || matches.size() != 1 + 11)
	{
		throw std::invalid_argument{ "invalid time point format" };
	}

	out.year    = string_to_number<int>(matches[1].str());
	out.month   = string_to_number<unsigned>(matches[2].str());
	out.day     = string_to_number<unsigned>(matches[3].str());
	out.hours   = string_to_number<unsigned>(matches[4].str());
	out.minutes = string_to_number<unsigned>(matches[5].str());
	out.seconds = string_to_number<unsigned>(matches[6].str());

	if (matches.length(7) > 1)
	{
		out.fracseconds = string_to_number<double>(matches[7].str());
	}

	if (matches.length(9))
	{
		const auto hours            = matches[9].str();
		const auto utc_offset_hours = string_to_number<int>((hours.front() == '+') ? &hours[1] : hours);
		out.utc_offset_minutes      = utc_offset_hours * 60;

		if (matches.length(11))
		{
			out.utc_offset_minutes.value() += string_to_number<int>(matches[11].str()) * (utc_offset_hours < 0 ? -1 : 1);
		}
	}

	return out;
}

struct parsed_date
{
	int      year;
	unsigned month;
	unsigned day;
};

parsed_date parse_date(std::string_view in)
{
	parsed_date out;

	static const std::regex re{ R"(^(-?\d{1,4})-(\d{2})-(\d{2})$)" };
	//                              1       2       3

	std::match_results<std::string_view::const_iterator> matches;
	if (!std::regex_match(in.begin(), in.end(), matches, re) || matches.size() != 1 + 3)
	{
		throw std::invalid_argument{ "invalid date format" };
	}

	out.year  = string_to_number<int>(matches[1].str());
	out.month = string_to_number<unsigned>(matches[2].str());
	out.day   = string_to_number<unsigned>(matches[3].str());

	return out;
}

struct parsed_time_of_day
{
	unsigned              hours;
	unsigned              minutes;
	unsigned              seconds;
	std::optional<double> fracseconds;
	std::optional<int>    utc_offset_minutes;
};

parsed_time_of_day parse_time_of_day(std::string_view in)
{
	parsed_time_of_day out;

	static const std::regex re{ R"(^(\d{2}):(\d{2}):(\d{2})(\.\d*)?( ?([+-]\d{2})(:?(\d{2}))?)?$)" };
	//                              1       2       3      4       5  6          7  8

	std::match_results<std::string_view::const_iterator> matches;
	if (!std::regex_match(in.begin(), in.end(), matches, re) || matches.size() != 1 + 8)
	{
		throw std::invalid_argument{ "invalid time of day format" };
	}

	out.hours   = string_to_number<unsigned>(matches[1].str());
	out.minutes = string_to_number<unsigned>(matches[2].str());
	out.seconds = string_to_number<unsigned>(matches[3].str());

	if (matches.length(4) > 1)
	{
		out.fracseconds = string_to_number<double>(matches[4].str());
	}

	if (matches.length(6))
	{
		const auto hours            = matches[6].str();
		const auto utc_offset_hours = string_to_number<int>((hours.front() == '+') ? &hours[1] : hours);
		out.utc_offset_minutes      = utc_offset_hours * 60;

		if (matches.length(8))
		{
			out.utc_offset_minutes.value() += string_to_number<int>(matches[8].str()) * (utc_offset_hours < 0 ? -1 : 1);
		}
	}

	return out;
}

} // namespace

void string_to_time_point(std::string_view in, time_point& out)
{
	// TODO: Use std::chrono::parse when it becomes available in libstdc++

	// https://github.com/HowardHinnant/date.git would be a great alternative,
	// but I prefer not to have any additional dependencies other than the database client libraries.

	const auto parsed = parse_time_point(in);

	out = std::chrono::sys_days{ std::chrono::year{ parsed.year } / std::chrono::month{ parsed.month } / parsed.day } +
	      std::chrono::hours{ parsed.hours } + std::chrono::minutes{ parsed.minutes } + std::chrono::seconds{ parsed.seconds };

	if (parsed.fracseconds)
	{
		out += std::chrono::microseconds{ static_cast<uint32_t>(parsed.fracseconds.value() * 1e6 + .5) };
	}

	if (parsed.utc_offset_minutes)
	{
		out -= std::chrono::minutes{ parsed.utc_offset_minutes.value() };
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

	const auto parsed = parse_date(in);

	out = std::chrono::sys_days{ std::chrono::year{ parsed.year } / std::chrono::month{ parsed.month } / parsed.day };
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

	const auto parsed = parse_time_of_day(in);

	std::chrono::microseconds tmp{ (3600LL * parsed.hours + 60 * parsed.minutes + parsed.seconds) * 1000000LL };

	if (parsed.fracseconds)
	{
		tmp += std::chrono::microseconds{ static_cast<uint32_t>(parsed.fracseconds.value() * 1e6 + .5) };
	}

	if (parsed.utc_offset_minutes)
	{
		tmp -= std::chrono::minutes{ parsed.utc_offset_minutes.value() };
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
		out.resize(10 * 3 + 19 * 4 + 6 + 1 + 1);
		std::sprintf(out.data(),
		             "%04d-%02u-%02u %02ld:%02ld:%02ld.%06ldZ",
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
		out.resize(10 * 3 + 19 * 3 + 5 + 1 + 1);
		std::sprintf(out.data(),
		             "%04d-%02u-%02u %02ld:%02ld:%02ldZ",
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

#ifdef SQUID_HAVE_BOOST_DATE_TIME
void boost_ptime_to_string(const boost::posix_time::ptime& in, std::string& out)
{
	out = boost_ptime_to_string(in);
}

std::string boost_ptime_to_string(const boost::posix_time::ptime& in)
{
	auto tmp = boost::posix_time::to_iso_extended_string(in);
	// Expected format of tmp is YYYY-MM-DDTHH:MM:SS[.fffffffff]
	// Format to return is YYYY-MM-DD HH:MM:SS.fffffffff
	// So just replace the 'T' with a space.
	if (tmp.length() >= 19u && tmp[10u] == 'T')
	{
		tmp[10u] = ' ';
	}
	return tmp + "Z";
}

void boost_date_to_string(const boost::gregorian::date& in, std::string& out)
{
	out = boost::gregorian::to_iso_extended_string(in);
}

std::string boost_date_to_string(const boost::gregorian::date& in)
{
	return boost::gregorian::to_iso_extended_string(in);
}

void boost_time_duration_to_string(const boost::posix_time::time_duration& in, std::string& out)
{
	out = boost::posix_time::to_simple_string(in);
}

std::string boost_time_duration_to_string(const boost::posix_time::time_duration& in)
{
	return boost::posix_time::to_simple_string(in);
}

void string_to_boost_ptime(std::string_view in, boost::posix_time::ptime& out)
{
	const auto parsed = parse_time_point(in);

	out = boost::posix_time::ptime{ boost::gregorian::date{ static_cast<short unsigned int>(parsed.year),
		                                                    static_cast<short unsigned int>(parsed.month),
		                                                    static_cast<short unsigned int>(parsed.day) },
		                            boost::posix_time::time_duration{ parsed.hours, parsed.minutes, parsed.seconds } };

	if (parsed.fracseconds)
	{
		out += boost::posix_time::microseconds{ static_cast<uint32_t>(parsed.fracseconds.value() * 1e6 + .5) };
	}

	if (parsed.utc_offset_minutes)
	{
		out -= boost::posix_time::minutes{ parsed.utc_offset_minutes.value() };
	}
}

boost::posix_time::ptime string_to_boost_ptime(std::string_view in)
{
	boost::posix_time::ptime out{};
	string_to_boost_ptime(in, out);
	return out;
}

void string_to_boost_date(std::string_view in, boost::gregorian::date& out)
{
	const auto parsed = parse_date(in);

	if (parsed.year < 0)
	{
		throw std::invalid_argument{ "boost date does not support negative years" };
	}

	out = boost::gregorian::date{ static_cast<short unsigned int>(parsed.year),
		                          static_cast<short unsigned int>(parsed.month),
		                          static_cast<short unsigned int>(parsed.day) };
}

boost::gregorian::date string_to_boost_date(std::string_view in)
{
	boost::gregorian::date out{};
	string_to_boost_date(in, out);
	return out;
}

void string_to_boost_time_duration(std::string_view in, boost::posix_time::time_duration& out)
{
	const auto parsed = parse_time_of_day(in);

	out = boost::posix_time::time_duration{ parsed.hours, parsed.minutes, parsed.seconds };

	if (parsed.fracseconds)
	{
		out += boost::posix_time::microseconds{ static_cast<uint32_t>(parsed.fracseconds.value() * 1e6 + .5) };
	}

	if (parsed.utc_offset_minutes)
	{
		out -= boost::posix_time::minutes{ parsed.utc_offset_minutes.value() };
	}
}

boost::posix_time::time_duration string_to_boost_time_duration(std::string_view in)
{
	boost::posix_time::time_duration out;
	string_to_boost_time_duration(in, out);
	return out;
}
#endif

} // namespace squid
