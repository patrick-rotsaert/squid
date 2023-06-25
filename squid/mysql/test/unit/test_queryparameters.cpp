//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/mysql/detail/queryparameters.h>
#include <squid/mysql/detail/query.h>
#include <squid/mysql/detail/conversions.h>
#include <squid/detail/conversions.h>
#include <mysql/mysql.h>

namespace squid {
namespace mysql {

namespace {

class QueryParameterBinder
{
	mysql_query                       q_;
	std::map<std::string, parameter>  p_;
	std::unique_ptr<query_parameters> qp_;

public:
	template<typename T>
	explicit QueryParameterBinder(const T& value)
	    : q_{ "SELECT :first" }
	    , p_{}
	    , qp_{}
	{
		this->p_.insert_or_assign("first", parameter{ value, parameter::by_value{} });
		this->qp_ = std::make_unique<query_parameters>(this->q_, this->p_);
		EXPECT_EQ(this->qp_->binds().size(), 1u);
	}

	const MYSQL_BIND& bind() const
	{
		return this->qp_->binds().front();
	}
};

} // namespace

TEST(MysqlQueryparametersTest, NoStatementParamsAndNoQueryParams)
{
	mysql_query                      q{ "" };
	std::map<std::string, parameter> p{};
	query_parameters                 qp{ q, p };
	EXPECT_TRUE(qp.binds().empty());
}

TEST(y, MoreQueryParamsThanStatementParamsMustBeAllowed)
{
	mysql_query                      q{ "" };
	std::map<std::string, parameter> p{};
	p.insert_or_assign("first", parameter{ 1, parameter::by_value{} });
	query_parameters qp{ q, p };
	EXPECT_TRUE(qp.binds().empty());
}

TEST(MysqlQueryparametersTest, MoreStatementParamsThanQueryParamsMustNotBeAllowed)
{
	mysql_query                      q{ "SELECT :first, :second" };
	std::map<std::string, parameter> p{};
	p.insert_or_assign("first", parameter{ 1, parameter::by_value{} });
	EXPECT_ANY_THROW((query_parameters{ q, p }));
}

TEST(MysqlQueryparametersTest, NoneParameter)
{
	QueryParameterBinder qpb{ std::nullopt };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_NULL);
	EXPECT_EQ(bind.buffer_length, 0ul);
	EXPECT_EQ(bind.buffer, nullptr);
}

TEST(MysqlQueryparametersTest, BooleanParameter)
{
	{
		QueryParameterBinder qpb{ true };
		const auto&          bind = qpb.bind();

		EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_TINY);
		EXPECT_NE(bind.buffer, nullptr);
		EXPECT_TRUE(*reinterpret_cast<const bool*>(bind.buffer));
	}
	{
		QueryParameterBinder qpb{ false };
		const auto&          bind = qpb.bind();

		EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_TINY);
		EXPECT_NE(bind.buffer, nullptr);
		EXPECT_FALSE(*reinterpret_cast<const bool*>(bind.buffer));
	}
}

TEST(MysqlQueryparametersTest, CharParameter)
{
	QueryParameterBinder qpb{ 'X' };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_STRING);
	EXPECT_NE(bind.buffer, nullptr);
	EXPECT_EQ(bind.buffer_length, 1ul);
	EXPECT_EQ(*reinterpret_cast<const char*>(bind.buffer), 'X');
}

TEST(MysqlQueryparametersTest, IntParameter)
{
	auto&& test = [](const auto value, enum enum_field_types buffer_type, bool is_unsigned) {
		QueryParameterBinder qpb{ value };
		const auto&          bind = qpb.bind();

		EXPECT_EQ(bind.buffer_type, buffer_type);
		EXPECT_EQ(bind.is_unsigned, is_unsigned);
		EXPECT_NE(bind.buffer, nullptr);
		EXPECT_EQ(*reinterpret_cast<std::add_pointer_t<decltype(value)>>(bind.buffer), value);
	};

	test(static_cast<signed char>(42), MYSQL_TYPE_TINY, false);
	test(static_cast<unsigned char>(255), MYSQL_TYPE_TINY, true);
	test(static_cast<int16_t>(0x7fff), MYSQL_TYPE_SHORT, false);
	test(static_cast<uint16_t>(0xffff), MYSQL_TYPE_SHORT, true);
	test(static_cast<int32_t>(0x7fffffff), MYSQL_TYPE_LONG, false);
	test(static_cast<uint32_t>(0xffffffff), MYSQL_TYPE_LONG, true);
	test(static_cast<int64_t>(0x7fffffffffffffff), MYSQL_TYPE_LONGLONG, false);
	test(static_cast<uint64_t>(0xffffffffffffffff), MYSQL_TYPE_LONGLONG, true);
}

TEST(MysqlQueryparametersTest, FloatParameter)
{
	auto&& test = [](const auto value, enum enum_field_types buffer_type) {
		QueryParameterBinder qpb{ value };
		const auto&          bind = qpb.bind();

		EXPECT_EQ(bind.buffer_type, buffer_type);
		EXPECT_NE(bind.buffer, nullptr);
		EXPECT_EQ(*reinterpret_cast<std::add_pointer_t<decltype(value)>>(bind.buffer), value);
	};

	test(42.42f, MYSQL_TYPE_FLOAT);
	test(42.42, MYSQL_TYPE_DOUBLE);
}

TEST(MysqlQueryparametersTest, StringParameter)
{
	auto&& test = [](const auto value) {
		QueryParameterBinder qpb{ value };
		const auto&          bind = qpb.bind();

		EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_STRING);
		EXPECT_NE(bind.buffer, nullptr);
		EXPECT_EQ(bind.buffer_length, value.length());
		EXPECT_EQ((std::string_view{ reinterpret_cast<const char*>(bind.buffer), bind.buffer_length }), value);
	};

	test(std::string{ "foo" });
	test(std::string_view{ "foo" });
}

TEST(MysqlQueryparametersTest, ByteStringParameter)
{
	auto&& test = [](const auto value) {
		QueryParameterBinder qpb{ value };
		const auto&          bind = qpb.bind();

		EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_BLOB);
		EXPECT_NE(bind.buffer, nullptr);
		EXPECT_EQ(bind.buffer_length, value.length());
		EXPECT_EQ((byte_string_view{ reinterpret_cast<const unsigned char*>(bind.buffer), bind.buffer_length }), value);
	};

	unsigned char    data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
	byte_string_view bv{ data, 4u };

	test(bv);
	test(byte_string{ bv });
}

TEST(MysqlQueryparametersTest, TimepointParameter)
{
	constexpr auto       tp = "2023-06-25 15:27:19.123456";
	QueryParameterBinder qpb{ string_to_time_point(tp) };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_DATETIME);
	EXPECT_EQ(bind.buffer_length, sizeof(MYSQL_TIME));
	EXPECT_NE(bind.buffer, nullptr);
	time_point out;
	from_mysql_time(*reinterpret_cast<const MYSQL_TIME*>(bind.buffer), out);
	EXPECT_EQ(time_point_to_string(out), tp);
}

TEST(MysqlQueryparametersTest, DateParameter)
{
	constexpr auto       dt = "2023-06-25";
	QueryParameterBinder qpb{ string_to_date(dt) };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_DATE);
	EXPECT_EQ(bind.buffer_length, sizeof(MYSQL_TIME));
	EXPECT_NE(bind.buffer, nullptr);
	date out;
	from_mysql_time(*reinterpret_cast<const MYSQL_TIME*>(bind.buffer), out);
	EXPECT_EQ(date_to_string(out), dt);
}

TEST(MysqlQueryparametersTest, TimeofdayParameter)
{
	constexpr auto       tm = "15:27:19.112233";
	QueryParameterBinder qpb{ string_to_time_of_day(tm) };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_TIME);
	EXPECT_EQ(bind.buffer_length, sizeof(MYSQL_TIME));
	EXPECT_NE(bind.buffer, nullptr);
	time_of_day out;
	from_mysql_time(*reinterpret_cast<const MYSQL_TIME*>(bind.buffer), out);
	EXPECT_EQ(time_of_day_to_string(out), tm);
}

#ifdef SQUID_HAVE_BOOST_DATE_TIME
TEST(MysqlQueryparametersTest, BoostPtimeParameter)
{
	constexpr auto       tp = "2023-06-25 15:27:19.123123";
	QueryParameterBinder qpb{ string_to_boost_ptime(tp) };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_DATETIME);
	EXPECT_EQ(bind.buffer_length, sizeof(MYSQL_TIME));
	EXPECT_NE(bind.buffer, nullptr);
	time_point out;
	from_mysql_time(*reinterpret_cast<const MYSQL_TIME*>(bind.buffer), out);
	EXPECT_EQ(time_point_to_string(out), tp);
}

TEST(MysqlQueryparametersTest, BoostDateParameter)
{
	constexpr auto       dt = "2023-06-25";
	QueryParameterBinder qpb{ string_to_boost_date(dt) };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_DATE);
	EXPECT_EQ(bind.buffer_length, sizeof(MYSQL_TIME));
	EXPECT_NE(bind.buffer, nullptr);
	date out;
	from_mysql_time(*reinterpret_cast<const MYSQL_TIME*>(bind.buffer), out);
	EXPECT_EQ(date_to_string(out), dt);
}

TEST(MysqlQueryparametersTest, BoostTimedurationParameter)
{
	constexpr auto       tm = "15:27:19.000111";
	QueryParameterBinder qpb{ string_to_boost_time_duration(tm) };
	const auto&          bind = qpb.bind();

	EXPECT_EQ(bind.buffer_type, MYSQL_TYPE_TIME);
	EXPECT_EQ(bind.buffer_length, sizeof(MYSQL_TIME));
	EXPECT_NE(bind.buffer, nullptr);
	time_of_day out;
	from_mysql_time(*reinterpret_cast<const MYSQL_TIME*>(bind.buffer), out);
	EXPECT_EQ(time_of_day_to_string(out), tm);
}
#endif

} // namespace mysql
} // namespace squid
