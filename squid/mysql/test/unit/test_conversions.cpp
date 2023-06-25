//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/mysql/detail/conversions.h>
#include <squid/detail/conversions.h>

namespace squid {
namespace mysql {

TEST(MysqlConversionsTest, DateToMysqlTime)
{
	MYSQL_TIME out{};
	date_to_mysql_time(string_to_date("2023-06-25"), out);

	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 0);
	EXPECT_EQ(out.minute, 0);
	EXPECT_EQ(out.second, 0);
	EXPECT_EQ(out.second_part, 0);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, DateBCToMysqlTime)
{
	MYSQL_TIME out{};
	date_to_mysql_time(string_to_date("-123-06-25"), out);

	EXPECT_TRUE(out.neg);
	EXPECT_EQ(out.year, 123);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 0);
	EXPECT_EQ(out.minute, 0);
	EXPECT_EQ(out.second, 0);
	EXPECT_EQ(out.second_part, 0);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, TimeofdayToMysqlTime)
{
	MYSQL_TIME out{};
	time_of_day_to_mysql_time(string_to_time_of_day("16:12:46"), out);

	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 0);
	EXPECT_EQ(out.month, 0);
	EXPECT_EQ(out.day, 0);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 0);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, TimeofdayWithFractionalSecondsToMysqlTime)
{
	MYSQL_TIME out{};
	time_of_day_to_mysql_time(string_to_time_of_day("16:12:46.123456"), out);

	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 0);
	EXPECT_EQ(out.month, 0);
	EXPECT_EQ(out.day, 0);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromDate)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_date("2023-06-25"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_DATE);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 0);
	EXPECT_EQ(out.minute, 0);
	EXPECT_EQ(out.second, 0);
	EXPECT_EQ(out.second_part, 0);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromTimeofday)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_time_of_day("16:12:46.123456"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_TIME);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 0);
	EXPECT_EQ(out.month, 0);
	EXPECT_EQ(out.day, 0);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromTimepoint)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_time_point("2023-06-25 16:12:46.123456"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_DATETIME);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromTimepointAtTimezone)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_time_point("2023-06-25 16:12:46.123456+02:00"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_DATETIME);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 16 - 2);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, FromMysqlTimeToDate)
{
	MYSQL_TIME in{};
	in.year  = 2023;
	in.month = 6;
	in.day   = 25;
	date out{};
	from_mysql_time(in, out);
	EXPECT_EQ(out, string_to_date("2023-06-25"));
}

TEST(MysqlConversionsTest, FromMysqlTimeToTimeOfDay)
{
	MYSQL_TIME in{};
	in.hour        = 16;
	in.minute      = 12;
	in.second      = 46;
	in.second_part = 123456;
	time_of_day out{};
	from_mysql_time(in, out);
	EXPECT_EQ(out.to_duration(), string_to_time_of_day("16:12:46.123456").to_duration());
}

TEST(MysqlConversionsTest, FromMysqlTimeToTimePoint)
{
	MYSQL_TIME in{};
	in.year                   = 2023;
	in.month                  = 6;
	in.day                    = 25;
	in.hour                   = 16;
	in.minute                 = 12;
	in.second                 = 46;
	in.second_part            = 123456;
	in.time_zone_displacement = 3600;
	time_point out{};
	from_mysql_time(in, out);
	EXPECT_EQ(out, string_to_time_point("2023-06-25 16:12:46.123456+01:00"));
}

#ifdef SQUID_HAVE_BOOST_DATE_TIME

TEST(MysqlConversionsTest, BoostDateToMysqlTime)
{
	MYSQL_TIME out{};
	boost_date_to_mysql_time(string_to_boost_date("2023-06-25"), out);

	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 0);
	EXPECT_EQ(out.minute, 0);
	EXPECT_EQ(out.second, 0);
	EXPECT_EQ(out.second_part, 0);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, BoostTimedurationToMysqlTime)
{
	MYSQL_TIME out{};
	boost_time_duration_to_mysql_time(string_to_boost_time_duration("16:12:46"), out);

	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 0);
	EXPECT_EQ(out.month, 0);
	EXPECT_EQ(out.day, 0);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 0);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, BoostTimedurationWithFractionalSecondsToMysqlTime)
{
	MYSQL_TIME out{};
	boost_time_duration_to_mysql_time(string_to_boost_time_duration("16:12:46.123456"), out);

	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 0);
	EXPECT_EQ(out.month, 0);
	EXPECT_EQ(out.day, 0);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromBoostDate)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_boost_date("2023-06-25"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_DATE);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 0);
	EXPECT_EQ(out.minute, 0);
	EXPECT_EQ(out.second, 0);
	EXPECT_EQ(out.second_part, 0);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromBoostTimeduration)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_boost_time_duration("16:12:46.123456"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_TIME);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 0);
	EXPECT_EQ(out.month, 0);
	EXPECT_EQ(out.day, 0);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromBoostPtime)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_boost_ptime("2023-06-25 16:12:46.123456"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_DATETIME);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 16);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, ToMysqlTimeFromBoostPtimeAtTimezone)
{
	MYSQL_TIME out{};
	to_mysql_time(string_to_boost_ptime("2023-06-25 16:12:46.123456+02:00"), out);

	EXPECT_EQ(out.time_type, MYSQL_TIMESTAMP_DATETIME);
	EXPECT_FALSE(out.neg);
	EXPECT_EQ(out.year, 2023);
	EXPECT_EQ(out.month, 6);
	EXPECT_EQ(out.day, 25);
	EXPECT_EQ(out.hour, 16 - 2);
	EXPECT_EQ(out.minute, 12);
	EXPECT_EQ(out.second, 46);
	EXPECT_EQ(out.second_part, 123456);
	EXPECT_EQ(out.time_zone_displacement, 0);
}

TEST(MysqlConversionsTest, FromMysqlTimeToBoostDate)
{
	MYSQL_TIME in{};
	in.year  = 2023;
	in.month = 6;
	in.day   = 25;
	boost::gregorian::date out{};
	from_mysql_time(in, out);
	EXPECT_EQ(out, string_to_boost_date("2023-06-25"));
}

TEST(MysqlConversionsTest, FromMysqlTimeToBoostTimeduration)
{
	MYSQL_TIME in{};
	in.hour        = 16;
	in.minute      = 12;
	in.second      = 46;
	in.second_part = 123456;
	boost::posix_time::time_duration out{};
	from_mysql_time(in, out);
	EXPECT_EQ(out, string_to_boost_time_duration("16:12:46.123456"));
}

TEST(MysqlConversionsTest, FromMysqlTimeToBoostPtime)
{
	MYSQL_TIME in{};
	in.year                   = 2023;
	in.month                  = 6;
	in.day                    = 25;
	in.hour                   = 16;
	in.minute                 = 12;
	in.second                 = 46;
	in.second_part            = 123456;
	in.time_zone_displacement = 3600;
	boost::posix_time::ptime out{};
	from_mysql_time(in, out);
	EXPECT_EQ(out, string_to_boost_ptime("2023-06-25 16:12:46.123456+01:00"));
}
#endif

} // namespace mysql
} // namespace squid
