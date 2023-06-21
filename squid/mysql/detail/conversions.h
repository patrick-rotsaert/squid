//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/config.h"
#include "squid/types.h"

#include <mysql/mysql.h>

#ifdef SQUID_HAVE_BOOST_DATE_TIME
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#endif

namespace squid {
namespace mysql {

void date_to_mysql_time(const date& in, MYSQL_TIME& out);

void time_of_day_to_mysql_time(const time_of_day& in, MYSQL_TIME& out);

void to_mysql_time(const date& in, MYSQL_TIME& out);

void to_mysql_time(const time_of_day& in, MYSQL_TIME& out);

void to_mysql_time(const time_point& in, MYSQL_TIME& out);

#ifdef SQUID_HAVE_BOOST_DATE_TIME
void boost_date_to_mysql_time(const boost::gregorian::date& in, MYSQL_TIME& out);

void boost_time_duration_to_mysql_time(const boost::posix_time::time_duration& in, MYSQL_TIME& out);

void to_mysql_time(const boost::gregorian::date& in, MYSQL_TIME& out);

void to_mysql_time(const boost::posix_time::time_duration& in, MYSQL_TIME& out);

void to_mysql_time(const boost::posix_time::ptime& in, MYSQL_TIME& out);
#endif

void from_mysql_time(const MYSQL_TIME& in, date& out);

void from_mysql_time(const MYSQL_TIME& in, time_of_day& out);

void from_mysql_time(const MYSQL_TIME& in, time_point& out);

#ifdef SQUID_HAVE_BOOST_DATE_TIME
void from_mysql_time(const MYSQL_TIME& in, boost::gregorian::date& out);

void from_mysql_time(const MYSQL_TIME& in, boost::posix_time::time_duration& out);

void from_mysql_time(const MYSQL_TIME& in, boost::posix_time::ptime& out);
#endif

} // namespace mysql
} // namespace squid
