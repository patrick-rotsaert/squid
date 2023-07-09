//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/sqlite3/error.h>
#include <squid/sqlite3/detail/sqliteapimock.h>

namespace squid {
namespace sqlite {

TEST(ErrorTests, TestConstructionWithMessage)
{
	auto e = error{ "the message" };

	EXPECT_STREQ(e.what(), "the message");
	EXPECT_FALSE(e.ec().has_value());
}

TEST(ErrorTests, TestConstructionWithApiMessageAndErrorCode)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, errstr(testing::Eq(42))).WillOnce(testing::Return("the detail for ec 42"));

	auto e = error{ api, "the message", 42 };

	EXPECT_STREQ(e.what(), "the message: the detail for ec 42");
	EXPECT_TRUE(e.ec().has_value());
	EXPECT_EQ(e.ec().value(), 42);
}

TEST(ErrorTests, TestConstructionWithApiMessageAndConnection)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, errmsg(testing::Eq(sqlite_api_mock::test_connection))).WillOnce(testing::Return("the detail for db"));
	EXPECT_CALL(api, errcode(testing::Eq(sqlite_api_mock::test_connection))).WillOnce(testing::Return(42));

	auto e = error{ api, "the message", *sqlite_api_mock::test_connection };

	EXPECT_STREQ(e.what(), "the message: the detail for db");
	EXPECT_TRUE(e.ec().has_value());
	EXPECT_EQ(e.ec().value(), 42);
}

TEST(ErrorTests, TestApiErrmsgReturnsNullptr)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, errmsg(testing::Eq(sqlite_api_mock::test_connection))).WillOnce(testing::ReturnNull());
	EXPECT_CALL(api, errcode(testing::Eq(sqlite_api_mock::test_connection))).WillOnce(testing::Return(42));

	auto e = error{ api, "the message", *sqlite_api_mock::test_connection };

	EXPECT_STREQ(e.what(), "the message: <null>");
	EXPECT_TRUE(e.ec().has_value());
	EXPECT_EQ(e.ec().value(), 42);
}

TEST(ErrorTests, TestApiErrstrReturnsNullptr)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, errstr(testing::Eq(42))).WillOnce(testing::ReturnNull());

	auto e = error{ api, "the message", 42 };

	EXPECT_STREQ(e.what(), "the message: <null>");
	EXPECT_TRUE(e.ec().has_value());
	EXPECT_EQ(e.ec().value(), 42);
}

} // namespace sqlite
} // namespace squid
