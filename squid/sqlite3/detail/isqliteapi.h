//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "sqlite3fwd.h"

#include <cstdint>

namespace squid {
namespace sqlite {

class isqlite_api
{
public:
	isqlite_api();
	virtual ~isqlite_api();

	isqlite_api(isqlite_api&&)      = delete;
	isqlite_api(const isqlite_api&) = delete;

	isqlite_api& operator=(isqlite_api&&)      = delete;
	isqlite_api& operator=(const isqlite_api&) = delete;

	virtual int open(const char* filename, sqlite3** ppDb) = 0;
	virtual int close(sqlite3* db)                         = 0;

	virtual int64_t changes64(sqlite3* db) = 0;

	virtual int prepare_v2(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail) = 0;
	virtual int finalize(sqlite3_stmt* pStmt)                                                                    = 0;
	virtual int step(sqlite3_stmt* pStmt)                                                                        = 0;
	virtual int reset(sqlite3_stmt* pStmt)                                                                       = 0;

	virtual int bind_parameter_index(sqlite3_stmt* pStmt, const char* zName)                                        = 0;
	virtual int bind_null(sqlite3_stmt* pStmt, int index)                                                           = 0;
	virtual int bind_int(sqlite3_stmt* pStmt, int index, int value)                                                 = 0;
	virtual int bind_int64(sqlite3_stmt* pStmt, int index, int64_t value)                                           = 0;
	virtual int bind_double(sqlite3_stmt* pStmt, int index, double value)                                           = 0;
	virtual int bind_text(sqlite3_stmt* pStmt, int index, const char* value, int length, void (*destructor)(void*)) = 0;
	virtual int bind_blob(sqlite3_stmt* pStmt, int index, const void* value, int length, void (*destructor)(void*)) = 0;

	virtual int                  column_int(sqlite3_stmt* pStmt, int index)    = 0;
	virtual int64_t              column_int64(sqlite3_stmt* pStmt, int index)  = 0;
	virtual double               column_double(sqlite3_stmt* pStmt, int index) = 0;
	virtual const unsigned char* column_text(sqlite3_stmt* pStmt, int index)   = 0;
	virtual const void*          column_blob(sqlite3_stmt* pStmt, int index)   = 0;
	virtual int                  column_bytes(sqlite3_stmt* pStmt, int index)  = 0;

	virtual int         column_count(sqlite3_stmt* pStmt)           = 0;
	virtual const char* column_name(sqlite3_stmt* pStmt, int index) = 0;
	virtual int         column_type(sqlite3_stmt* pStmt, int index) = 0;

	virtual int         errcode(sqlite3* db) = 0;
	virtual const char* errstr(int ec)       = 0;
	virtual const char* errmsg(sqlite3* db)  = 0;
};

} // namespace sqlite
} // namespace squid
