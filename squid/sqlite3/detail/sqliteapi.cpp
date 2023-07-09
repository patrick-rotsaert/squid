//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "sqliteapi.h"

#include <sqlite3.h>

namespace squid {
namespace sqlite {

sqlite_api::sqlite_api()
{
}

sqlite_api::~sqlite_api()
{
}

int sqlite_api::open(const char* filename, sqlite3** ppDb)
{
	return sqlite3_open(filename, ppDb);
}

int sqlite_api::close(sqlite3* db)
{
	return sqlite3_close(db);
}

int64_t sqlite_api::changes64(sqlite3* db)
{
	return static_cast<int64_t>(sqlite3_changes64(db));
}

int sqlite_api::prepare_v2(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail)
{
	return sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
}

int sqlite_api::finalize(sqlite3_stmt* pStmt)
{
	return sqlite3_finalize(pStmt);
}

int sqlite_api::step(sqlite3_stmt* pStmt)
{
	return sqlite3_step(pStmt);
}

int sqlite_api::reset(sqlite3_stmt* pStmt)
{
	return sqlite3_reset(pStmt);
}

int sqlite_api::bind_parameter_index(sqlite3_stmt* pStmt, const char* zName)
{
	return sqlite3_bind_parameter_index(pStmt, zName);
}

int sqlite_api::bind_null(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_bind_null(pStmt, index);
}

int sqlite_api::bind_int(sqlite3_stmt* pStmt, int index, int value)
{
	return sqlite3_bind_int(pStmt, index, value);
}

int sqlite_api::bind_int64(sqlite3_stmt* pStmt, int index, int64_t value)
{
	return sqlite3_bind_int64(pStmt, index, static_cast<sqlite3_int64>(value));
}

int sqlite_api::bind_double(sqlite3_stmt* pStmt, int index, double value)
{
	return sqlite3_bind_double(pStmt, index, value);
}

int sqlite_api::bind_text(sqlite3_stmt* pStmt, int index, const char* value, int length, void (*destructor)(void*))
{
	return sqlite3_bind_text(pStmt, index, value, length, destructor);
}

int sqlite_api::bind_blob(sqlite3_stmt* pStmt, int index, const void* value, int length, void (*destructor)(void*))
{
	return sqlite3_bind_blob(pStmt, index, value, length, destructor);
}

int sqlite_api::column_int(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_column_int(pStmt, index);
}

int64_t sqlite_api::column_int64(sqlite3_stmt* pStmt, int index)
{
	return static_cast<int64_t>(sqlite3_column_int64(pStmt, index));
}

double sqlite_api::column_double(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_column_double(pStmt, index);
}

const unsigned char* sqlite_api::column_text(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_column_text(pStmt, index);
}

const void* sqlite_api::column_blob(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_column_blob(pStmt, index);
}

int sqlite_api::column_bytes(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_column_bytes(pStmt, index);
}

int sqlite_api::column_count(sqlite3_stmt* pStmt)
{
	return sqlite3_column_count(pStmt);
}

const char* sqlite_api::column_name(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_column_name(pStmt, index);
}

int sqlite_api::column_type(sqlite3_stmt* pStmt, int index)
{
	return sqlite3_column_type(pStmt, index);
}

int sqlite_api::errcode(sqlite3* db)
{
	return sqlite3_errcode(db);
}

const char* sqlite_api::errstr(int ec)
{
	return sqlite3_errstr(ec);
}

const char* sqlite_api::errmsg(sqlite3* db)
{
	return sqlite3_errmsg(db);
}

} // namespace sqlite
} // namespace squid
