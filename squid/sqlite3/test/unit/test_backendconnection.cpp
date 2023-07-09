//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/sqlite3/backendconnection.h>
#include <squid/sqlite3/detail/sqliteapimock.h>
#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

static constexpr auto g_connection_info = "the connection info";
static constexpr auto g_query           = "select foo from bar";

void set_connection_handle(const char*, sqlite3** db)
{
	*db = sqlite_api_mock::test_connection;
}

void set_statement_handle(sqlite3*, const char*, int, sqlite3_stmt** ppStmt, const char**)
{
	*ppStmt = sqlite_api_mock::test_statement;
}

} // namespace

TEST(BackendConnectionTests, TestOpenAndClose)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, open(testing::StrEq(g_connection_info), testing::NotNull()))
	    .WillOnce(testing::DoAll(&set_connection_handle, testing::Return(SQLITE_OK)));

	EXPECT_CALL(api, close(sqlite_api_mock::test_connection)).Times(1);

	auto c = backend_connection{ api, g_connection_info };
	EXPECT_EQ(&c.handle(), sqlite_api_mock::test_connection);
}

TEST(BackendConnectionTests, TestOpenReturnsError)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, open(testing::StrEq(g_connection_info), testing::NotNull())).WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_CALL(api, close(sqlite_api_mock::test_connection)).Times(0);

	EXPECT_ANY_THROW((backend_connection{ api, g_connection_info }));
}

TEST(BackendConnectionTests, TestOpenDoesNotSetHandle)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, open(testing::StrEq(g_connection_info), testing::NotNull())).WillOnce(testing::Return(SQLITE_OK));

	EXPECT_CALL(api, close(sqlite_api_mock::test_connection)).Times(0);

	EXPECT_ANY_THROW((backend_connection{ api, g_connection_info }));
}

TEST(BackendConnectionTests, TestExecuteQuery)
{
	auto&& test_with_step_result = [](int step_result) {
		auto api = sqlite_api_mock_nice{};

		{
			auto seq = testing::Sequence{};

			EXPECT_CALL(api, open(testing::StrEq(g_connection_info), testing::NotNull()))
			    .WillOnce(testing::DoAll(&set_connection_handle, testing::Return(SQLITE_OK)));

			EXPECT_CALL(api, prepare_v2(sqlite_api_mock::test_connection, testing::StrEq(g_query), -1, testing::NotNull(), nullptr))
			    .WillOnce(testing::DoAll(&set_statement_handle, testing::Return(SQLITE_OK)));

			EXPECT_CALL(api, step(sqlite_api_mock::test_statement)).WillOnce(testing::Return(step_result));

			EXPECT_CALL(api, finalize(sqlite_api_mock::test_statement)).Times(1);

			EXPECT_CALL(api, close(sqlite_api_mock::test_connection)).Times(1);
		}

		backend_connection{ api, g_connection_info }.execute(g_query);
	};

	test_with_step_result(SQLITE_DONE);
	test_with_step_result(SQLITE_ROW);
	EXPECT_ANY_THROW(test_with_step_result(SQLITE_ERROR));
}

} // namespace sqlite
} // namespace squid
