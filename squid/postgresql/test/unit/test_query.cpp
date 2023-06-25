//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/postgresql/detail/query.h>

namespace squid {
namespace postgresql {

TEST(PostgresqlQueryTest, EmptyString)
{
	postgresql_query q{ "" };
	EXPECT_EQ(q.query(), "");
	EXPECT_EQ(q.parameter_count(), 0);
}

TEST(PostgresqlQueryTest, OneParameter)
{
	postgresql_query q{ "SELECT :first" };
	EXPECT_EQ(q.query(), "SELECT $1");
	EXPECT_EQ(q.parameter_count(), 1);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 1u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], 1);
}

TEST(PostgresqlQueryTest, TwoParameters)
{
	postgresql_query q{ "SELECT :first, :second" };
	EXPECT_EQ(q.query(), "SELECT $1, $2");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], 1);
	EXPECT_EQ(map["second"], 2);
}

TEST(PostgresqlQueryTest, ParametersWithSameName)
{
	postgresql_query q{ "SELECT :first, :second, :first" };
	EXPECT_EQ(q.query(), "SELECT $1, $2, $1");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], 1);
	EXPECT_EQ(map["second"], 2);
}

TEST(PostgresqlQueryTest, ParametersWithSameNameBegin)
{
	postgresql_query q{ "SELECT :first, :second, :first_not_same" };
	EXPECT_EQ(q.query(), "SELECT $1, $2, $3");
	EXPECT_EQ(q.parameter_count(), 3);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 3u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], 1);
	EXPECT_EQ(map["second"], 2);
	EXPECT_EQ(map["first_not_same"], 3);
}

TEST(PostgresqlQueryTest, ParametersWithAlternateSyntax)
{
	postgresql_query q{ "SELECT :first, $second, @first FROM foo" };
	EXPECT_EQ(q.query(), "SELECT $1, $2, $1 FROM foo");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], 1);
	EXPECT_EQ(map["second"], 2);
}

TEST(PostgresqlQueryTest, ParameterInStringLiteral)
{
	postgresql_query q{ "SELECT :first, :second, ':third'" };
	EXPECT_EQ(q.query(), "SELECT $1, $2, ':third'");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], 1);
	EXPECT_EQ(map["second"], 2);
}

TEST(PostgresqlQueryTest, ParameterInIdentifier)
{
	postgresql_query q{ "SELECT :first, :second, \":third\"" };
	EXPECT_EQ(q.query(), "SELECT $1, $2, \":third\"");
	EXPECT_EQ(q.parameter_count(), 2);
	EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
	auto map = q.parameter_name_pos_map();
	EXPECT_EQ(map["first"], 1);
	EXPECT_EQ(map["second"], 2);
}

TEST(PostgresqlQueryTest, CastingOperator)
{
	{
		postgresql_query q{ "SELECT :first::xxx, :second" };
		EXPECT_EQ(q.query(), "SELECT $1::xxx, $2");
		EXPECT_EQ(q.parameter_count(), 2);
		EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
		auto map = q.parameter_name_pos_map();
		EXPECT_EQ(map["first"], 1);
		EXPECT_EQ(map["second"], 2);
	}
	{
		postgresql_query q{ "SELECT :first :: xxx, :second" };
		EXPECT_EQ(q.query(), "SELECT $1 :: xxx, $2");
		EXPECT_EQ(q.parameter_count(), 2);
		EXPECT_EQ(q.parameter_name_pos_map().size(), 2u);
		auto map = q.parameter_name_pos_map();
		EXPECT_EQ(map["first"], 1);
		EXPECT_EQ(map["second"], 2);
	}
}

} // namespace postgresql
} // namespace squid
