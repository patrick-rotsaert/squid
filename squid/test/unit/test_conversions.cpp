#include <gtest/gtest.h>
#include <squid/detail/conversions.h>
#include <cmath>

namespace squid {

namespace {

time_point make_time_point(int year, unsigned month, unsigned day, unsigned hours, unsigned minutes, unsigned seconds)
{
	return std::chrono::sys_days{ std::chrono::year{ year } / std::chrono::month{ month } / day } + std::chrono::hours{ hours } +
	       std::chrono::minutes{ minutes } + std::chrono::seconds{ seconds };
}

time_point make_time_point_micro(int year, unsigned month, unsigned day, unsigned hours, unsigned minutes, double seconds)
{
	assert(seconds >= 0);
	double     intseconds{};
	const auto fracseconds = std::modf(seconds, &intseconds);
	return make_time_point(year, month, day, hours, minutes, std::lround(intseconds)) +
	       std::chrono::microseconds{ std::lround(fracseconds * 1e6) };
}

date make_date(int year, unsigned month, unsigned day)
{
	return std::chrono::sys_days{ std::chrono::year{ year } / std::chrono::month{ month } / day };
}

time_of_day make_time_of_day(unsigned hours, unsigned minutes, unsigned seconds)
{
	return time_of_day{ std::chrono::hours{ hours } + std::chrono::minutes{ minutes } + std::chrono::seconds{ seconds } };
}

time_of_day make_time_of_day_micro(unsigned hours, unsigned minutes, double seconds)
{
	assert(seconds >= 0);
	double     intseconds{};
	const auto fracseconds = std::modf(seconds, &intseconds);
	return time_of_day{ std::chrono::hours{ hours } + std::chrono::minutes{ minutes } + std::chrono::seconds{ std::lround(intseconds) } +
		                std::chrono::microseconds{ std::lround(fracseconds * 1e6) } };
}

} // namespace

TEST(ConversionsTest, StringToNumberSuccess)
{
	EXPECT_EQ(string_to_number<int>("42"), 42);
	EXPECT_EQ(string_to_number<int>("-42"), -42);
	EXPECT_DOUBLE_EQ(string_to_number<double>("42.42"), 42.42);
	EXPECT_DOUBLE_EQ(string_to_number<double>("-42.42"), -42.42);
	EXPECT_DOUBLE_EQ(string_to_number<double>("1.234e3"), 1.234e3);
	EXPECT_DOUBLE_EQ(string_to_number<double>("-1.234e3"), -1.234e3);
	EXPECT_DOUBLE_EQ(string_to_number<double>("-1.234e-3"), -1.234e-3);
}

TEST(ConversionsTest, StringToNumberInvalidInput)
{
	EXPECT_THROW(string_to_number<int>("abc"), std::system_error);
	EXPECT_THROW(string_to_number<int>(""), std::system_error);
	EXPECT_THROW(string_to_number<int>(" 42"), std::system_error);
	EXPECT_THROW(string_to_number<double>("abc"), std::system_error);
	EXPECT_THROW(string_to_number<double>(""), std::system_error);
	EXPECT_THROW(string_to_number<double>(" 42.42"), std::system_error);
}

TEST(ConversionsTest, StringToNumberIncompleteConversion)
{
	EXPECT_THROW(string_to_number<int>("42a"), std::invalid_argument);
	EXPECT_THROW(string_to_number<int>("42 "), std::invalid_argument);
	EXPECT_THROW(string_to_number<double>("42.42a"), std::invalid_argument);
	EXPECT_THROW(string_to_number<double>("42.42 "), std::invalid_argument);
}

TEST(ConversionsTest, StringToTimePointMinimal)
{
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45"), make_time_point(2022, 3, 18, 23, 59, 45));
}

TEST(ConversionsTest, StringToTimePointWithFractionalSeconds)
{
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45."), make_time_point_micro(2022, 3, 18, 23, 59, 45.));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.1"), make_time_point_micro(2022, 3, 18, 23, 59, 45.1));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.12"), make_time_point_micro(2022, 3, 18, 23, 59, 45.12));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123"), make_time_point_micro(2022, 3, 18, 23, 59, 45.123));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.1234"), make_time_point_micro(2022, 3, 18, 23, 59, 45.1234));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.12345"), make_time_point_micro(2022, 3, 18, 23, 59, 45.12345));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456"), make_time_point_micro(2022, 3, 18, 23, 59, 45.123456));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.1234567"), make_time_point_micro(2022, 3, 18, 23, 59, 45.123457));
}

TEST(ConversionsTest, StringToTimePointWithUtcOffsetHoursOnly)
{
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45+04"), make_time_point(2022, 3, 18, 23, 59, 45) - std::chrono::hours{ 4 });
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45-04"), make_time_point(2022, 3, 18, 23, 59, 45) + std::chrono::hours{ 4 });
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45 +04"), make_time_point(2022, 3, 18, 23, 59, 45) - std::chrono::hours{ 4 });
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45 -04"), make_time_point(2022, 3, 18, 23, 59, 45) + std::chrono::hours{ 4 });
}

TEST(ConversionsTest, StringToTimePointWithUtcOffsetHoursAndMinutes)
{
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45+0430"),
	          make_time_point(2022, 3, 18, 23, 59, 45) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45-0430"),
	          make_time_point(2022, 3, 18, 23, 59, 45) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45 +0430"),
	          make_time_point(2022, 3, 18, 23, 59, 45) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45 -0430"),
	          make_time_point(2022, 3, 18, 23, 59, 45) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));

	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45+04:30"),
	          make_time_point(2022, 3, 18, 23, 59, 45) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45-04:30"),
	          make_time_point(2022, 3, 18, 23, 59, 45) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45 +04:30"),
	          make_time_point(2022, 3, 18, 23, 59, 45) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45 -04:30"),
	          make_time_point(2022, 3, 18, 23, 59, 45) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
}

TEST(ConversionsTest, StringToTimePointWithFractionalSecondsAndUtcOffsetHoursAndMinutes)
{
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456+0430"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456-0430"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456 +0430"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456 -0430"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));

	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456+04:30"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456-04:30"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456 +04:30"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) - (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
	EXPECT_EQ(string_to_time_point("2022-03-18 23:59:45.123456 -04:30"),
	          make_time_point_micro(2022, 3, 18, 23, 59, 45.123456) + (std::chrono::hours{ 4 } + std::chrono::minutes{ 30 }));
}

TEST(ConversionsTest, StringToTimePointErrors)
{
	EXPECT_THROW(string_to_time_point(""), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:45.123456-04XX"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:45.123456-XX30"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:45.123456 0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:45.XXXXXX-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:45/123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:XX.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:XX:45.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 XX:59:45.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18*23:59:45.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-XX 23:59:45.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-XX-18 23:59:45.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("XXXX-03-18 23:59:45.123456-0430"), std::invalid_argument);

	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:45-043"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:45-4"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:4-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:5:45-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 2:59:45-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-1 23:59:45-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-3-18 23:59:45-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("222-03-18 23:59:45-0430"), std::invalid_argument);

	EXPECT_THROW(string_to_time_point("2022-03-18 23:59:"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:59"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23:"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03-18 23"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022-03"), std::invalid_argument);
	EXPECT_THROW(string_to_time_point("2022"), std::invalid_argument);
}

TEST(ConversionsTest, StringToDate)
{
	EXPECT_EQ(string_to_date("2022-03-18"), make_date(2022, 3, 18));
}

TEST(ConversionsTest, StringToDateErrors)
{
	EXPECT_THROW(string_to_date("XXXX-03-18"), std::invalid_argument);
	EXPECT_THROW(string_to_date("2022-XX-18"), std::invalid_argument);
	EXPECT_THROW(string_to_date("2022-03-XX"), std::invalid_argument);
	EXPECT_THROW(string_to_date("20220318"), std::invalid_argument);
	EXPECT_THROW(string_to_date("2022-03-1"), std::invalid_argument);
	EXPECT_THROW(string_to_date("2022-3-18"), std::invalid_argument);
	EXPECT_THROW(string_to_date("222-03-18"), std::invalid_argument);
	EXPECT_THROW(string_to_date("2022-03"), std::invalid_argument);
	EXPECT_THROW(string_to_date("2022"), std::invalid_argument);
	EXPECT_THROW(string_to_date(""), std::invalid_argument);
}

TEST(ConversionsTest, StringToTimeOfDayMinimal)
{
	EXPECT_EQ(string_to_time_of_day("23:59:45").to_duration(), make_time_of_day(23, 59, 45).to_duration());
}

TEST(ConversionsTest, StringToTimeOfDayWithFractionalSeconds)
{
	EXPECT_EQ(string_to_time_of_day("23:59:45.").to_duration(), make_time_of_day_micro(23, 59, 45.).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.1").to_duration(), make_time_of_day_micro(23, 59, 45.1).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.12").to_duration(), make_time_of_day_micro(23, 59, 45.12).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123").to_duration(), make_time_of_day_micro(23, 59, 45.123).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.1234").to_duration(), make_time_of_day_micro(23, 59, 45.1234).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.12345").to_duration(), make_time_of_day_micro(23, 59, 45.12345).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456").to_duration(), make_time_of_day_micro(23, 59, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.1234567").to_duration(), make_time_of_day_micro(23, 59, 45.123457).to_duration());
}

TEST(ConversionsTest, StringToTimeOfDayWithUtcOffsetHoursOnly)
{
	EXPECT_EQ(string_to_time_of_day("23:59:45+04").to_duration(), make_time_of_day(23 - 4, 59, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45-04").to_duration(), make_time_of_day(23 + 4, 59, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45 +04").to_duration(), make_time_of_day(23 - 4, 59, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45 -04").to_duration(), make_time_of_day(23 + 4, 59, 45).to_duration());
}

TEST(ConversionsTest, StringToTimeOfDayWithUtcOffsetHoursAndMinutes)
{
	EXPECT_EQ(string_to_time_of_day("23:59:45+0430").to_duration(), make_time_of_day(23 - 4, 59 - 30, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45-0430").to_duration(), make_time_of_day(23 + 4, 59 + 30, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45 +0430").to_duration(), make_time_of_day(23 - 4, 59 - 30, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45 -0430").to_duration(), make_time_of_day(23 + 4, 59 + 30, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45+04:30").to_duration(), make_time_of_day(23 - 4, 59 - 30, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45-04:30").to_duration(), make_time_of_day(23 + 4, 59 + 30, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45 +04:30").to_duration(), make_time_of_day(23 - 4, 59 - 30, 45).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45 -04:30").to_duration(), make_time_of_day(23 + 4, 59 + 30, 45).to_duration());
}

TEST(ConversionsTest, StringToTimeOfDayWithFractionalSecondsAndUtcOffsetHoursAndMinutes)
{
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456+0430").to_duration(),
	          make_time_of_day_micro(23 - 4, 59 - 30, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456-0430").to_duration(),
	          make_time_of_day_micro(23 + 4, 59 + 30, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456 +0430").to_duration(),
	          make_time_of_day_micro(23 - 4, 59 - 30, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456 -0430").to_duration(),
	          make_time_of_day_micro(23 + 4, 59 + 30, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456+04:30").to_duration(),
	          make_time_of_day_micro(23 - 4, 59 - 30, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456-04:30").to_duration(),
	          make_time_of_day_micro(23 + 4, 59 + 30, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456 +04:30").to_duration(),
	          make_time_of_day_micro(23 - 4, 59 - 30, 45.123456).to_duration());
	EXPECT_EQ(string_to_time_of_day("23:59:45.123456 -04:30").to_duration(),
	          make_time_of_day_micro(23 + 4, 59 + 30, 45.123456).to_duration());
}

TEST(ConversionsTest, StringToTimeOfDayErrors)
{
	EXPECT_THROW(string_to_time_of_day(""), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:45.123456-04XX"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:45.123456-XX30"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:45.123456 0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:45.XXXXXX-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:45/123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:XX.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:XX:45.123456-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("XX:59:45.123456-0430"), std::invalid_argument);

	EXPECT_THROW(string_to_time_of_day("23:59:45-043"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:45-4"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59:4-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:5:45-0430"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("2:59:45-0430"), std::invalid_argument);

	EXPECT_THROW(string_to_time_of_day("23:59:"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:59"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23:"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day("23"), std::invalid_argument);
	EXPECT_THROW(string_to_time_of_day(""), std::invalid_argument);
}

TEST(ConversionsTest, TimePointToString)
{
	EXPECT_EQ(time_point_to_string(make_time_point(2022, 3, 1, 3, 4, 5)), "2022-03-01 03:04:05");
	EXPECT_EQ(time_point_to_string(make_time_point_micro(2022, 3, 1, 3, 4, 5.12345)), "2022-03-01 03:04:05.123450");
	EXPECT_EQ(time_point_to_string(make_time_point_micro(2022, 3, 1, 3, 4, 5.123456)), "2022-03-01 03:04:05.123456");
	EXPECT_EQ(time_point_to_string(make_time_point_micro(2022, 3, 1, 3, 4, 5.1234567)), "2022-03-01 03:04:05.123457");
}

TEST(ConversionsTest, DateToString)
{
	EXPECT_EQ(date_to_string(make_date(2022, 3, 1)), "2022-03-01");
}

TEST(ConversionsTest, TimeOfDayToString)
{
	EXPECT_EQ(time_of_day_to_string(make_time_of_day(3, 4, 5)), "03:04:05");
	EXPECT_EQ(time_of_day_to_string(make_time_of_day_micro(3, 4, 5.12345)), "03:04:05.123450");
	EXPECT_EQ(time_of_day_to_string(make_time_of_day_micro(3, 4, 5.123456)), "03:04:05.123456");
	EXPECT_EQ(time_of_day_to_string(make_time_of_day_micro(3, 4, 5.1234567)), "03:04:05.123457");
}

} // namespace squid
