//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/mysql/detail/query.h>

namespace squid {
namespace mysql {

TEST(PostgresqlQueryTest, EmptyString)
{
	mysql_query q{ "" };
	EXPECT_EQ(q.query(), "");
	EXPECT_EQ(q.parameter_count(), 0);
}

TEST(PostgresqlQueryTest, OneParameter)
{
	mysql_query q{ "SELECT :first" };
	EXPECT_EQ(q.query(), "SELECT ?");
	EXPECT_EQ(q.parameter_count(), 1);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 1u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], std::vector<size_t>{ 0u });
}

TEST(PostgresqlQueryTest, TwoParameters)
{
	mysql_query q{ "SELECT :first, :second" };
	EXPECT_EQ(q.query(), "SELECT ?, ?");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], std::vector<size_t>{ 0u });
	EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
}

TEST(PostgresqlQueryTest, ParametersWithSameName)
{
	mysql_query q{ "SELECT :first, :second, :first" };
	EXPECT_EQ(q.query(), "SELECT ?, ?, ?");
	EXPECT_EQ(q.parameter_count(), 3);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], (std::vector<size_t>{ 0u, 2u }));
	EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
}

TEST(PostgresqlQueryTest, ParametersWithSameNameBegin)
{
	mysql_query q{ "SELECT :first, :second, :first_not_same" };
	EXPECT_EQ(q.query(), "SELECT ?, ?, ?");
	EXPECT_EQ(q.parameter_count(), 3);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 3u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], std::vector<size_t>{ 0u });
	EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
	EXPECT_EQ(map["first_not_same"], std::vector<size_t>{ 2u });
}

TEST(PostgresqlQueryTest, ParametersWithAlternateSyntax)
{
	mysql_query q{ "SELECT :first, $second, @first FROM foo" };
	EXPECT_EQ(q.query(), "SELECT ?, ?, ? FROM foo");
	EXPECT_EQ(q.parameter_count(), 3);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], (std::vector<size_t>{ 0u, 2u }));
	EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
}

TEST(PostgresqlQueryTest, ParameterInStringLiteral)
{
	mysql_query q{ "SELECT :first, :second, ':third'" };
	EXPECT_EQ(q.query(), "SELECT ?, ?, ':third'");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], std::vector<size_t>{ 0u });
	EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
}

TEST(PostgresqlQueryTest, ParameterInIdentifier)
{
	mysql_query q{ "SELECT :first, :second, \":third\"" };
	EXPECT_EQ(q.query(), "SELECT ?, ?, \":third\"");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], std::vector<size_t>{ 0u });
	EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
}

TEST(PostgresqlQueryTest, CastingOperator)
{
	{
		mysql_query q{ "SELECT :first::xxx, :second" };
		EXPECT_EQ(q.query(), "SELECT ?::xxx, ?");
		EXPECT_EQ(q.parameter_count(), 2);
		EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
		auto map = q.parameter_name_pos_map();
		EXPECT_EQ(map["first"], std::vector<size_t>{ 0u });
		EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
	}
	{
		mysql_query q{ "SELECT :first :: xxx, :second" };
		EXPECT_EQ(q.query(), "SELECT ? :: xxx, ?");
		EXPECT_EQ(q.parameter_count(), 2);
		EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
		auto map = q.parameter_name_pos_map();
		EXPECT_EQ(map["first"], std::vector<size_t>{ 0u });
		EXPECT_EQ(map["second"], std::vector<size_t>{ 1u });
	}
}

} // namespace mysql
} // namespace squid
