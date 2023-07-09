//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "isqliteapi.h"

namespace squid {
namespace sqlite {

class sqlite_api : public isqlite_api
{
public:
	sqlite_api();
	~sqlite_api() override;

	sqlite_api(sqlite_api&&)      = delete;
	sqlite_api(const sqlite_api&) = delete;

	sqlite_api& operator=(sqlite_api&&)      = delete;
	sqlite_api& operator=(const sqlite_api&) = delete;

	int open(const char* filename, sqlite3** ppDb) override;
	int close(sqlite3* db) override;

	int64_t changes64(sqlite3* db) override;

	int prepare_v2(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail) override;
	int finalize(sqlite3_stmt* pStmt) override;
	int step(sqlite3_stmt* pStmt) override;
	int reset(sqlite3_stmt* pStmt) override;

	int bind_parameter_index(sqlite3_stmt* pStmt, const char* zName) override;
	int bind_null(sqlite3_stmt* pStmt, int index) override;
	int bind_int(sqlite3_stmt* pStmt, int index, int value) override;
	int bind_int64(sqlite3_stmt* pStmt, int index, int64_t value) override;
	int bind_double(sqlite3_stmt* pStmt, int index, double value) override;
	int bind_text(sqlite3_stmt* pStmt, int index, const char* value, int length, void (*destructor)(void*)) override;
	int bind_blob(sqlite3_stmt* pStmt, int index, const void* value, int length, void (*destructor)(void*)) override;

	int                  column_int(sqlite3_stmt* pStmt, int index) override;
	int64_t              column_int64(sqlite3_stmt* pStmt, int index) override;
	double               column_double(sqlite3_stmt* pStmt, int index) override;
	const unsigned char* column_text(sqlite3_stmt* pStmt, int index) override;
	const void*          column_blob(sqlite3_stmt* pStmt, int index) override;
	int                  column_bytes(sqlite3_stmt* pStmt, int index) override;

	int         column_count(sqlite3_stmt* pStmt) override;
	const char* column_name(sqlite3_stmt* pStmt, int index) override;
	int         column_type(sqlite3_stmt* pStmt, int index) override;

	int         errcode(sqlite3* db) override;
	const char* errstr(int ec) override;
	const char* errmsg(sqlite3* db) override;
};

} // namespace sqlite
} // namespace squid
