//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/statement.h"
#include "squid/sqlite3/error.h"

#include "squid/detail/always_false.h"
#include "squid/detail/conversions.h"

#include <sstream>
#include <iomanip>
#include <cassert>

#ifdef SQUID_DEBUG_SQLITE3
#include <iostream>
#endif

namespace squid {
namespace sqlite {

namespace {

sqlite3_stmt* prepare_statement(sqlite3& connection, const std::string& query)
{
#ifdef SQUID_DEBUG_SQLITE3
	std::cout << "preparing: " << query << "\n";
#endif

	sqlite3_stmt* stmt{ nullptr };
	auto          rc = sqlite3_prepare_v2(&connection, query.c_str(), -1, &stmt, nullptr);

	if (SQLITE_OK != rc)
	{
		throw Error{ "sqlite3_prepare_v2 failed", connection };
	}
	else if (!stmt)
	{
		throw Error{ "sqlite3_prepare_v2 did not set the statement handle", connection };
	}
	else
	{
		return stmt;
	}
}

void bind_parameter(sqlite3& connection, sqlite3_stmt& statement, const std::string& name, const Parameter& parameter)
{
	auto tmpName        = ":" + name;
	auto parameterIndex = sqlite3_bind_parameter_index(&statement, tmpName.c_str());
	if (parameterIndex < 1)
	{
		// hacky!
		tmpName.front() = '@';
		parameterIndex  = sqlite3_bind_parameter_index(&statement, tmpName.c_str());
		if (parameterIndex < 1)
		{
			// hacky again ;)
			tmpName.front() = '$';
			parameterIndex  = sqlite3_bind_parameter_index(&statement, tmpName.c_str());
			if (parameterIndex < 1)
			{
				std::ostringstream error;
				error << "Parameter name " << std::quoted(name) << " was not found in the statement";
				throw Error{ error.str() };
			}
		}
	}
	assert(parameterIndex > 0);

#define BIND0(f)                                                                                                                           \
	do                                                                                                                                     \
	{                                                                                                                                      \
		if (SQLITE_OK != f(&statement, parameterIndex))                                                                                    \
		{                                                                                                                                  \
			throw Error(#f " failed", connection);                                                                                         \
		}                                                                                                                                  \
	} while (false)

#define BIND(f, ...)                                                                                                                       \
	do                                                                                                                                     \
	{                                                                                                                                      \
		if (SQLITE_OK != f(&statement, parameterIndex, __VA_ARGS__))                                                                       \
		{                                                                                                                                  \
			throw Error(#f " failed", connection);                                                                                         \
		}                                                                                                                                  \
	} while (false)

	std::visit(
	    [&connection, &statement, &parameterIndex](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, const std::nullopt_t*>)
		    {
			    BIND0(sqlite3_bind_null);
		    }
		    else if constexpr (std::is_same_v<T, const bool*>)
		    {
			    BIND(sqlite3_bind_int, *arg ? 1 : 0);
		    }
		    else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, const signed char*> ||
		                       std::is_same_v<T, const unsigned char*> || std::is_same_v<T, const std::int16_t*> ||
		                       std::is_same_v<T, const std::uint16_t*> || std::is_same_v<T, const std::int32_t*>)
		    {
			    BIND(sqlite3_bind_int, static_cast<int>(*arg));
		    }
		    else if constexpr (std::is_same_v<T, const std::uint32_t*> || std::is_same_v<T, const std::int64_t*> ||
		                       std::is_same_v<T, const std::uint64_t*>)
		    {
			    BIND(sqlite3_bind_int64, static_cast<sqlite3_int64>(*arg));
		    }
		    else if constexpr (std::is_same_v<T, const float*> || std::is_same_v<T, const double*> || std::is_same_v<T, const long double*>)
		    {
			    BIND(sqlite3_bind_double, static_cast<double>(*arg));
		    }
		    else if constexpr (std::is_same_v<T, const Parameter::enum_char*>)
		    {
			    BIND(sqlite3_bind_text, &arg->value, 1, SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const std::string*> || std::is_same_v<T, const std::string_view*>)
		    {
			    BIND(sqlite3_bind_text, arg->data(), arg->length(), SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const byte_string*> || std::is_same_v<T, const byte_string_view*>)
		    {
			    BIND(sqlite3_bind_blob, arg->data(), arg->length(), SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const std::chrono::system_clock::time_point*>)
		    {
			    auto tmp = time_point_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), tmp.length(), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const std::chrono::year_month_day*>)
		    {
			    auto tmp = year_month_day_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), tmp.length(), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const std::chrono::hh_mm_ss<std::chrono::microseconds>*>)
		    {
			    auto tmp = hh_mm_ss_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), tmp.length(), SQLITE_TRANSIENT);
		    }
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    parameter.pointer());
}

void bind_parameters(sqlite3& connection, sqlite3_stmt& statement, const std::map<std::string, Parameter>& parameters)
{
	for (const auto& pair : parameters)
	{
		bind_parameter(connection, statement, pair.first, pair.second);
	}
}

void store_string(sqlite3& connection, sqlite3_stmt& statement, int column, std::string_view columnName, std::string& out)
{
	const auto ptr = sqlite3_column_text(&statement, column);
	const auto len = sqlite3_column_bytes(&statement, column);
	if (!ptr)
	{
		std::ostringstream error;
		error << "sqlite3_column_text returned NULL for column " << std::quoted(columnName);
		throw Error{ error.str(), connection };
	}
	else if (len < 0)
	{
		std::ostringstream error;
		error << "sqlite3_column_bytes returned " << len << " for column " << std::quoted(columnName);
		throw Error{ error.str(), connection };
	}
	out.assign(reinterpret_cast<const char*>(ptr), len);
}

void store_result(sqlite3&                               connection,
                  sqlite3_stmt&                          statement,
                  const Result::non_nullable_value_type& result,
                  int                                    column,
                  std::string_view                       columnName,
                  int                                    columnType)
{
	assert(SQLITE_NULL != columnType);
	(void)columnType;

	std::visit(
	    [&](auto&& arg) {
		    auto& destination = *arg;
		    using T           = std::decay_t<decltype(destination)>;
		    if constexpr (std::is_same_v<T, bool>)
		    {
			    destination = sqlite3_column_int(&statement, column) ? true : false;
		    }
		    else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> ||
		                       std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::int32_t>)
		    {
			    destination = static_cast<T>(sqlite3_column_int(&statement, column));
		    }
		    else if constexpr (std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>)
		    {
			    destination = static_cast<T>(sqlite3_column_int64(&statement, column));
		    }
		    else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>)
		    {
			    destination = static_cast<T>(sqlite3_column_double(&statement, column));
		    }
		    else if constexpr (std::is_same_v<T, std::string>)
		    {
			    store_string(connection, statement, column, columnName, destination);
		    }
		    else if constexpr (std::is_same_v<T, byte_string>)
		    {
			    const auto ptr = sqlite3_column_blob(&statement, column);
			    const auto len = sqlite3_column_bytes(&statement, column);
			    if (!ptr && len == 0)
			    {
				    destination.clear();
			    }
			    else if (!ptr)
			    {
				    std::ostringstream error;
				    error << "sqlite3_column_blob returned NULL and sqlite3_column_bytes returned " << len << " for column "
				          << std::quoted(columnName);
				    throw Error{ error.str(), connection };
			    }
			    else if (len < 0)
			    {
				    std::ostringstream error;
				    error << "sqlite3_column_bytes returned " << len << " for column " << std::quoted(columnName);
				    throw Error{ error.str(), connection };
			    }
			    destination.assign(reinterpret_cast<const unsigned char*>(ptr), len);
		    }
		    else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column, columnName, tmp);
			    string_to_time_point(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, std::chrono::year_month_day>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column, columnName, tmp);
			    string_to_year_month_day(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, std::chrono::hh_mm_ss<std::chrono::microseconds>>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column, columnName, tmp);
			    string_to_hh_mm_ss(tmp, destination);
		    }
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    result);
}

void store_result(sqlite3& connection, sqlite3_stmt& statement, const Result& result, int column)
{
	const auto& destination = result.value();

	const auto columnName = sqlite3_column_name(&statement, column);
	assert(columnName);

	const auto columnType = sqlite3_column_type(&statement, column);

	if (SQLITE_NULL == columnType)
	{
		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, Result::non_nullable_value_type>)
			    {
				    std::ostringstream error;
				    error << "Cannot store a NULL value of field " << std::quoted(columnName) << " in a non-optional type";
				    throw Error{ error.str() };
			    }
			    else if constexpr (std::is_same_v<T, Result::nullable_value_type>)
			    {
				    std::visit(
				        [](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        *arg = std::nullopt;
				        },
				        arg);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    destination);
	}
	else
	{
		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, Result::non_nullable_value_type>)
			    {
				    store_result(connection, statement, arg, column, columnName, columnType);
			    }
			    else if constexpr (std::is_same_v<T, Result::nullable_value_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        using T = typename std::decay_t<decltype(*arg)>::value_type;
					        T tmp{};
					        store_result(connection, statement, Result::non_nullable_value_type{ &tmp }, column, columnName, columnType);
					        *arg = tmp;
				        },
				        arg);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    destination);
	}
}

void store_results(sqlite3& connection, sqlite3_stmt& statement, const std::vector<Result>& results)
{
	const auto columns = sqlite3_column_count(&statement);

	if (static_cast<int>(results.size()) > columns)
	{
		throw Error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a row with only " + std::to_string(columns) +
			         " column" + (columns == 1 ? "" : "s") };
	}

	int currentColumn = 0;
	for (const auto& result : results)
	{
		assert(currentColumn < columns);
		store_result(connection, statement, result, currentColumn++);
	}
}

} // namespace

Statement::Statement(std::shared_ptr<sqlite3> connection, std::string_view query, bool reuseStatement)
    : IBackendStatement{}
    , connection_{ connection }
    , query_{ query }
    , reuseStatement_{ reuseStatement }
    , statement_{}
    , stepResult_{ -1 }
{
	assert(this->connection_);
}

void Statement::execute(const std::map<std::string, Parameter>& parameters)
{
	assert(this->connection_);

	if (this->statement_)
	{
		if (this->reuseStatement_)
		{
			if (SQLITE_OK != sqlite3_reset(this->statement_.get()))
			{
				throw Error{ "sqlite3_reset failed", *this->connection_ };
			}
		}
		else
		{
			this->statement_.reset();
		}
	}

	if (!this->statement_)
	{
		this->statement_.reset(prepare_statement(*this->connection_, this->query_), sqlite3_finalize);
	}

	bind_parameters(*this->connection_, *this->statement_, parameters);

	this->stepResult_ = sqlite3_step(this->statement_.get());

	if (SQLITE_DONE != this->stepResult_ && SQLITE_ROW != this->stepResult_)
	{
		this->statement_.reset();
		throw Error{ "sqlite3_step failed", *this->connection_ };
	}
}

bool Statement::fetch(const std::vector<Result>& results)
{
	assert(this->connection_);

	if (!this->statement_)
	{
		throw Error{ "Cannot fetch row from a statement that has not been executed" };
	}

	if (SQLITE_DONE == this->stepResult_)
	{
		return false;
	}
	assert(SQLITE_ROW == this->stepResult_);

	store_results(*this->connection_, *this->statement_, results);

	this->stepResult_ = sqlite3_step(this->statement_.get());

	if (SQLITE_DONE != this->stepResult_ && SQLITE_ROW != this->stepResult_)
	{
		throw Error{ "sqlite3_step failed", *this->connection_ };
	}

	return true;
}

void Statement::execute(sqlite3& connection, const std::string& query)
{
	std::shared_ptr<sqlite3_stmt> statement{ prepare_statement(connection, query), sqlite3_finalize };

	auto rc = sqlite3_step(statement.get());
	if (SQLITE_DONE != rc && SQLITE_ROW != rc)
	{
		throw Error{ "sqlite3_step failed", connection };
	}
}

} // namespace sqlite
} // namespace squid
