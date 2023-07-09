//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "sqliteapimock.h"

namespace squid {
namespace sqlite {

namespace {

sqlite3      g_connection;
sqlite3_stmt g_statement;

} // namespace

sqlite3*      sqlite_api_mock::test_connection = &g_connection;
sqlite3_stmt* sqlite_api_mock::test_statement  = &g_statement;

std::shared_ptr<sqlite3>      sqlite_api_mock::test_connection_shared = std::make_shared<sqlite3>();
std::shared_ptr<sqlite3_stmt> sqlite_api_mock::test_statement_shared  = std::make_shared<sqlite3_stmt>();

sqlite_api_mock::sqlite_api_mock()
{
}

sqlite_api_mock::~sqlite_api_mock()
{
}

} // namespace sqlite
} // namespace squid
