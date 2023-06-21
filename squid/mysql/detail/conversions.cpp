//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "conversions.h"
#include "squid/mysql/error.h"

#ifdef SQUID_HAVE_BOOST_DATE_TIME
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#endif

namespace squid {
namespace mysql {

void date_to_mysql_time(const date& in, MYSQL_TIME& out)
{
	out.neg   = static_cast<int>(in.year()) < 0;
	out.year  = static_cast<unsigned int>(std::abs(static_cast<int>(in.year())));
	out.month = static_cast<unsigned int>(in.month());
	out.day   = static_cast<unsigned int>(in.day());
}

void time_of_day_to_mysql_time(const time_of_day& in, MYSQL_TIME& out)
{
	out.hour        = static_cast<unsigned int>(in.hours().count());
	out.minute      = static_cast<unsigned int>(in.minutes().count());
	out.second      = static_cast<unsigned int>(in.seconds().count());
	out.second_part = static_cast<unsigned long>(in.subseconds().count());
}

void to_mysql_time(const date& in, MYSQL_TIME& out)
{
	out.time_type = MYSQL_TIMESTAMP_DATE;
	date_to_mysql_time(in, out);
}

void to_mysql_time(const time_of_day& in, MYSQL_TIME& out)
{
	out.time_type = MYSQL_TIMESTAMP_TIME;
	time_of_day_to_mysql_time(in, out);
}

void to_mysql_time(const time_point& in, MYSQL_TIME& out)
{
	using namespace std::chrono;
	auto           dp = floor<days>(in);
	year_month_day date{ dp };
	hh_mm_ss       time{ floor<microseconds>(in - dp) };

	out.time_type = MYSQL_TIMESTAMP_DATETIME;
	date_to_mysql_time(date, out);
	time_of_day_to_mysql_time(time, out);
}

#ifdef SQUID_HAVE_BOOST_DATE_TIME
void boost_date_to_mysql_time(const boost::gregorian::date& in, MYSQL_TIME& out)
{
	out.year  = static_cast<unsigned int>(std::abs(static_cast<int>(in.year())));
	out.month = static_cast<unsigned int>(in.month());
	out.day   = static_cast<unsigned int>(in.day());
}

void boost_time_duration_to_mysql_time(const boost::posix_time::time_duration& in, MYSQL_TIME& out)
{
	out.hour   = static_cast<unsigned int>(in.hours());
	out.minute = static_cast<unsigned int>(in.minutes());
	out.second = static_cast<unsigned int>(in.seconds());
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
	out.second_part = static_cast<unsigned long>(in.fractional_seconds() / 1000);
#else
	out.second_part = static_cast<unsigned long>(in.fractional_seconds());
#endif
}

void to_mysql_time(const boost::gregorian::date& in, MYSQL_TIME& out)
{
	out.time_type = MYSQL_TIMESTAMP_DATE;
	boost_date_to_mysql_time(in, out);
}

void to_mysql_time(const boost::posix_time::time_duration& in, MYSQL_TIME& out)
{
	out.time_type = MYSQL_TIMESTAMP_TIME;
	boost_time_duration_to_mysql_time(in, out);
}

void to_mysql_time(const boost::posix_time::ptime& in, MYSQL_TIME& out)
{
	out.time_type = MYSQL_TIMESTAMP_DATETIME;
	boost_date_to_mysql_time(in.date(), out);
	boost_time_duration_to_mysql_time(in.time_of_day(), out);
}
#endif

void from_mysql_time(const MYSQL_TIME& in, date& out)
{
	out = std::chrono::sys_days{ std::chrono::year{ in.neg ? -static_cast<int>(in.year) : static_cast<int>(in.year) } /
		                         std::chrono::month{ in.month } / in.day };
}

void from_mysql_time(const MYSQL_TIME& in, time_of_day& out)
{
	const auto tmp = std::chrono::hours{ in.hour } + std::chrono::minutes{ in.minute } + std::chrono::seconds{ in.second } +
	                 std::chrono::microseconds{ in.second_part };

	out = time_of_day{ tmp };
}

void from_mysql_time(const MYSQL_TIME& in, time_point& out)
{
	out = std::chrono::sys_days{ std::chrono::year{ in.neg ? -static_cast<int>(in.year) : static_cast<int>(in.year) } /
		                         std::chrono::month{ in.month } / in.day } +
	      std::chrono::hours{ in.hour } + std::chrono::minutes{ in.minute } + std::chrono::seconds{ in.second } +
	      std::chrono::microseconds{ in.second_part } - std::chrono::seconds{ in.time_zone_displacement };
}

#ifdef SQUID_HAVE_BOOST_DATE_TIME
void from_mysql_time(const MYSQL_TIME& in, boost::gregorian::date& out)
{
	if (in.neg)
	{
		throw error{ "Boost date does not support negative years" };
	}
	out = boost::gregorian::date{ static_cast<short unsigned int>(in.year),
		                          static_cast<short unsigned int>(in.month),
		                          static_cast<short unsigned int>(in.day) };
}

void from_mysql_time(const MYSQL_TIME& in, boost::posix_time::time_duration& out)
{
	out = boost::posix_time::time_duration{ in.hour, in.minute, in.second } + boost::posix_time::microseconds{ in.second_part };
}

void from_mysql_time(const MYSQL_TIME& in, boost::posix_time::ptime& out)
{
	if (in.neg)
	{
		throw error{ "Boost date does not support negative years" };
	}
	out = boost::posix_time::ptime{ boost::gregorian::date{ static_cast<short unsigned int>(in.year),
		                                                    static_cast<short unsigned int>(in.month),
		                                                    static_cast<short unsigned int>(in.day) },
		                            boost::posix_time::time_duration{ in.hour, in.minute, in.second } +
		                                boost::posix_time::microseconds{ in.second_part } -
		                                boost::posix_time::seconds{ in.time_zone_displacement } };
}
#endif

} // namespace mysql
} // namespace squid
