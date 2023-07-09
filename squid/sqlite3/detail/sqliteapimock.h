//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "isqliteapi.h"

#include <memory>
#include <gmock/gmock.h>

struct sqlite3
{
};

struct sqlite3_stmt
{
};

namespace squid {
namespace sqlite {

class sqlite_api_mock : public isqlite_api
{
public:
	static sqlite3*      test_connection;
	static sqlite3_stmt* test_statement;

	static std::shared_ptr<sqlite3>      test_connection_shared;
	static std::shared_ptr<sqlite3_stmt> test_statement_shared;

	sqlite_api_mock();
	~sqlite_api_mock() override;

	sqlite_api_mock(sqlite_api_mock&&)      = delete;
	sqlite_api_mock(const sqlite_api_mock&) = delete;

	sqlite_api_mock& operator=(sqlite_api_mock&&)      = delete;
	sqlite_api_mock& operator=(const sqlite_api_mock&) = delete;

	MOCK_METHOD(int, open, (const char* filename, sqlite3** ppDb), (override));

	MOCK_METHOD(int, close, (sqlite3 * db), (override));

	MOCK_METHOD(int64_t, changes64, (sqlite3 * db), (override));

	MOCK_METHOD(int, prepare_v2, (sqlite3 * db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail), (override));
	MOCK_METHOD(int, finalize, (sqlite3_stmt * pStmt), (override));
	MOCK_METHOD(int, step, (sqlite3_stmt * pStmt), (override));
	MOCK_METHOD(int, reset, (sqlite3_stmt * pStmt), (override));

	MOCK_METHOD(int, bind_parameter_index, (sqlite3_stmt * pStmt, const char* zName), (override));
	MOCK_METHOD(int, bind_null, (sqlite3_stmt * pStmt, int index), (override));
	MOCK_METHOD(int, bind_int, (sqlite3_stmt * pStmt, int index, int value), (override));
	MOCK_METHOD(int, bind_int64, (sqlite3_stmt * pStmt, int index, int64_t value), (override));
	MOCK_METHOD(int, bind_double, (sqlite3_stmt * pStmt, int index, double value), (override));
	MOCK_METHOD(int, bind_text, (sqlite3_stmt * pStmt, int index, const char* value, int length, void (*destructor)(void*)), (override));
	MOCK_METHOD(int, bind_blob, (sqlite3_stmt * pStmt, int index, const void* value, int length, void (*destructor)(void*)), (override));

	MOCK_METHOD(int, column_int, (sqlite3_stmt * pStmt, int index), (override));
	MOCK_METHOD(int64_t, column_int64, (sqlite3_stmt * pStmt, int index), (override));
	MOCK_METHOD(double, column_double, (sqlite3_stmt * pStmt, int index), (override));
	MOCK_METHOD(const unsigned char*, column_text, (sqlite3_stmt * pStmt, int index), (override));
	MOCK_METHOD(const void*, column_blob, (sqlite3_stmt * pStmt, int index), (override));
	MOCK_METHOD(int, column_bytes, (sqlite3_stmt * pStmt, int index), (override));

	MOCK_METHOD(int, column_count, (sqlite3_stmt * pStmt), (override));
	MOCK_METHOD(const char*, column_name, (sqlite3_stmt * pStmt, int index), (override));
	MOCK_METHOD(int, column_type, (sqlite3_stmt * pStmt, int index), (override));

	MOCK_METHOD(int, errcode, (sqlite3 * db), (override));
	MOCK_METHOD(const char*, errstr, (int ec), (override));
	MOCK_METHOD(const char*, errmsg, (sqlite3 * db), (override));
};

using sqlite_api_mock_nice   = testing::NiceMock<sqlite_api_mock>;
using sqlite_api_mock_naggy  = testing::NaggyMock<sqlite_api_mock>;
using sqlite_api_mock_strict = testing::StrictMock<sqlite_api_mock>;

} // namespace sqlite
} // namespace squid

// This regex search and replace patterns can help to convert declarations into MOCK_METHOD macro calls
// Search pattern: ^(.+)\s([a-zA-Z_][a-zA-Z0-9_]*)\s*(\([^)]*\))\s*override\s*;\s*$
// Replace pattern: MOCK_METHOD(\1,\2,\3,(override));
