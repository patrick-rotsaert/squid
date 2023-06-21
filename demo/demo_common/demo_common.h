//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/connection.h"

namespace squid {
namespace demo {

enum MyEnum
{
	MyEnum_FIRST  = 42,
	MyEnum_SECOND = 43,
	MyEnum_THIRD  = 44,
};

enum class MyCharEnum : char
{
	FIRST  = 'a',
	SECOND = 'b',
	THIRD  = 'c',
};

enum class MyIntEnum : int
{
	FIRST  = 42,
	SECOND = 43,
	THIRD  = 44,
};

enum class Backend
{
	SQLITE3,
	POSTGRESQL,
	MYSQL
};

void demo_bindings(connection& connection);
void demo_field_info(connection& connection);
void demo_result_by_name(connection& connection);
void demo_query_stream(connection& connection);
void demo_bind_struct(connection& connection);
void demo_table_ops(connection& connection, Backend backend);

void demo_all(connection& connection, Backend backend);

} // namespace demo

} // namespace squid
