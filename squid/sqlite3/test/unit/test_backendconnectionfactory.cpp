//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/sqlite3/backendconnectionfactory.h>
#include <squid/sqlite3/backendconnection.h>
#include <squid/sqlite3/detail/sqliteapimock.h>
#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

static constexpr auto g_connection_info = "the connection info";

void set_connection_handle(const char*, sqlite3** db)
{
	*db = sqlite_api_mock::test_connection;
}

} // namespace

TEST(BackendConnectionFactoryTests, TestCreateBackendConnection)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, open(testing::StrEq(g_connection_info), testing::NotNull()))
	    .WillOnce(testing::DoAll(&set_connection_handle, testing::Return(SQLITE_OK)));

	EXPECT_CALL(api, close(sqlite_api_mock::test_connection)).Times(1);

	auto virtconn = backend_connection_factory{ api }.create_backend_connection(g_connection_info);
	auto conn     = std::dynamic_pointer_cast<backend_connection>(virtconn);
	EXPECT_NE(conn, nullptr);

	EXPECT_EQ(&conn->handle(), sqlite_api_mock::test_connection);
}

} // namespace sqlite
} // namespace squid
