//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/postgresql/detail/queryparameters.h>
#include <squid/postgresql/detail/query.h>
#include <squid/detail/conversions.h>

namespace squid {
namespace postgresql {

namespace {

template<typename T>
std::string get_one_query_parameter(const T& value)
{
	postgresql_query                 q{ "SELECT :first" };
	std::map<std::string, parameter> p{};
	p.insert_or_assign("first", parameter{ value, parameter::by_value{} });
	query_parameters qp{ q, p };
	EXPECT_EQ(qp.parameter_count(), 1);
	EXPECT_NE(qp.parameter_values(), nullptr);
	return qp.parameter_values()[0];
}

} // namespace

TEST(PostgresqlQueryparametersTest, NoStatementParamsAndNoQueryParams)
{
	postgresql_query                 q{ "" };
	std::map<std::string, parameter> p{};
	query_parameters                 qp{ q, p };
	EXPECT_EQ(qp.parameter_count(), 0);
}

TEST(PostgresqlQueryparametersTest, MoreQueryParamsThanStatementParamsMustBeAllowed)
{
	postgresql_query                 q{ "" };
	std::map<std::string, parameter> p{};
	p.insert_or_assign("first", parameter{ 1, parameter::by_value{} });
	query_parameters qp{ q, p };
	EXPECT_EQ(qp.parameter_count(), 0);
}

TEST(PostgresqlQueryparametersTest, MoreStatementParamsThanQueryParamsMustNotBeAllowed)
{
	postgresql_query                 q{ "SELECT :first, :second" };
	std::map<std::string, parameter> p{};
	p.insert_or_assign("first", parameter{ 1, parameter::by_value{} });
	EXPECT_ANY_THROW((query_parameters{ q, p }));
}

TEST(PostgresqlQueryparametersTest, NoneParameter)
{
	postgresql_query                 q{ "SELECT :first" };
	std::map<std::string, parameter> p{};
	p.insert_or_assign("first", parameter{ std::nullopt, parameter::by_value{} });
	query_parameters qp{ q, p };
	EXPECT_EQ(qp.parameter_count(), 1);
	EXPECT_NE(qp.parameter_values(), nullptr);
	EXPECT_EQ(qp.parameter_values()[0], nullptr);
}

TEST(PostgresqlQueryparametersTest, BooleanParameter)
{
	EXPECT_EQ(get_one_query_parameter(true), "t");
	EXPECT_EQ(get_one_query_parameter(false), "f");
}

TEST(PostgresqlQueryparametersTest, CharParameter)
{
	EXPECT_EQ(get_one_query_parameter('X'), "X");
}

TEST(PostgresqlQueryparametersTest, IntParameter)
{
	EXPECT_EQ(get_one_query_parameter(static_cast<signed char>(42)), "42");
	EXPECT_EQ(get_one_query_parameter(static_cast<unsigned char>(255)), "255");
	EXPECT_EQ(get_one_query_parameter(static_cast<int16_t>(0x7fff)), "32767");
	EXPECT_EQ(get_one_query_parameter(static_cast<uint16_t>(0xffff)), "65535");
	EXPECT_EQ(get_one_query_parameter(static_cast<int32_t>(0x7fffffff)), "2147483647");
	EXPECT_EQ(get_one_query_parameter(static_cast<uint32_t>(0xffffffff)), "4294967295");
	EXPECT_EQ(get_one_query_parameter(static_cast<int64_t>(0x7fffffffffffffff)), "9223372036854775807");
	EXPECT_EQ(get_one_query_parameter(static_cast<uint64_t>(0xffffffffffffffff)), "18446744073709551615");
}

TEST(PostgresqlQueryparametersTest, FloatParameter)
{
	EXPECT_EQ(get_one_query_parameter(42.42f), "42.42");
	EXPECT_EQ(get_one_query_parameter(42.42), "42.42");
}

TEST(PostgresqlQueryparametersTest, StringParameter)
{
	EXPECT_EQ(get_one_query_parameter(std::string{ "foo" }), "foo");
	EXPECT_EQ(get_one_query_parameter(std::string_view{ "bar" }), "bar");
}

TEST(PostgresqlQueryparametersTest, ByteStringParameter)
{
	unsigned char    data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
	byte_string_view bv{ data, 4u };
	EXPECT_EQ(get_one_query_parameter(bv), "\\xDEADBEEF");
	EXPECT_EQ(get_one_query_parameter(byte_string{ bv }), "\\xDEADBEEF");
}

TEST(PostgresqlQueryparametersTest, TimepointParameter)
{
	constexpr auto tp = "2023-06-25 15:27:19";
	EXPECT_EQ(get_one_query_parameter(string_to_time_point(tp)), tp);
}

TEST(PostgresqlQueryparametersTest, DateParameter)
{
	constexpr auto dt = "2023-06-25";
	EXPECT_EQ(get_one_query_parameter(string_to_date(dt)), dt);
}

TEST(PostgresqlQueryparametersTest, TimeofdayParameter)
{
	constexpr auto tm = "15:27:19";
	EXPECT_EQ(get_one_query_parameter(string_to_time_of_day(tm)), tm);
}

#ifdef SQUID_HAVE_BOOST_DATE_TIME
TEST(PostgresqlQueryparametersTest, BoostPtimeParameter)
{
	constexpr auto tp = "2023-06-25 15:27:19";
	EXPECT_EQ(get_one_query_parameter(string_to_boost_ptime(tp)), tp);
}

TEST(PostgresqlQueryparametersTest, BoostDateParameter)
{
	constexpr auto dt = "2023-06-25";
	EXPECT_EQ(get_one_query_parameter(string_to_boost_date(dt)), dt);
}

TEST(PostgresqlQueryparametersTest, BoostTimedurationParameter)
{
	constexpr auto tm = "15:27:19";
	EXPECT_EQ(get_one_query_parameter(string_to_boost_time_duration(tm)), tm);
}
#endif

} // namespace postgresql
} // namespace squid
