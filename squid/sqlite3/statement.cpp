//
// Copyright (C) 2022-2023 Patrick Rotsaert
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

#include <sqlite3.h>

#ifdef SQUID_DEBUG_SQLITE
#include <iostream>
#endif

namespace squid {
namespace sqlite {

namespace {

sqlite3_stmt* prepare_statement(sqlite3& connection, const std::string& query)
{
#ifdef SQUID_DEBUG_SQLITE
	std::cout << "preparing: " << query << "\n";
#endif

	sqlite3_stmt* stmt{ nullptr };
	auto          rc = sqlite3_prepare_v2(&connection, query.c_str(), -1, &stmt, nullptr);

	if (SQLITE_OK != rc)
	{
		throw error{ "sqlite3_prepare_v2 failed", connection };
	}
	else if (!stmt)
	{
		throw error{ "sqlite3_prepare_v2 did not set the statement handle", connection };
	}
	else
	{
		return stmt;
	}
}

void bind_parameter(sqlite3& connection, sqlite3_stmt& statement, const std::string& name, const parameter& parameter)
{
	auto tmp_name        = ":" + name;
	auto parameter_index = sqlite3_bind_parameter_index(&statement, tmp_name.c_str());
	if (parameter_index < 1)
	{
		// hacky!
		tmp_name.front() = '@';
		parameter_index  = sqlite3_bind_parameter_index(&statement, tmp_name.c_str());
		if (parameter_index < 1)
		{
			// hacky again ;)
			tmp_name.front() = '$';
			parameter_index  = sqlite3_bind_parameter_index(&statement, tmp_name.c_str());
			if (parameter_index < 1)
			{
				std::ostringstream msg;
				msg << "Parameter name " << std::quoted(name) << " was not found in the statement";
				throw error{ msg.str() };
			}
		}
	}
	assert(parameter_index > 0);

#define BIND0(f)                                                                                                                           \
	do                                                                                                                                     \
	{                                                                                                                                      \
		if (SQLITE_OK != f(&statement, parameter_index))                                                                                   \
		{                                                                                                                                  \
			throw error(#f " failed", connection);                                                                                         \
		}                                                                                                                                  \
	} while (false)

#define BIND(f, ...)                                                                                                                       \
	do                                                                                                                                     \
	{                                                                                                                                      \
		if (SQLITE_OK != f(&statement, parameter_index, __VA_ARGS__))                                                                      \
		{                                                                                                                                  \
			throw error(#f " failed", connection);                                                                                         \
		}                                                                                                                                  \
	} while (false)

	std::visit(
	    [&connection, &statement, &parameter_index](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, const std::nullopt_t*>)
		    {
			    BIND0(sqlite3_bind_null);
		    }
		    else if constexpr (std::is_same_v<T, const bool*>)
		    {
			    BIND(sqlite3_bind_int, *arg ? 1 : 0);
		    }
		    else if constexpr (std::is_same_v<T, const char*>)
		    {
			    BIND(sqlite3_bind_text, arg, 1, SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const signed char*> || std::is_same_v<T, const unsigned char*> ||
		                       std::is_same_v<T, const std::int16_t*> || std::is_same_v<T, const std::uint16_t*> ||
		                       std::is_same_v<T, const std::int32_t*>)
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
		    else if constexpr (std::is_same_v<T, const std::string*> || std::is_same_v<T, const std::string_view*>)
		    {
			    BIND(sqlite3_bind_text, arg->data(), static_cast<int>(arg->length()), SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const byte_string*> || std::is_same_v<T, const byte_string_view*>)
		    {
			    BIND(sqlite3_bind_blob, arg->data(), static_cast<int>(arg->length()), SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const time_point*>)
		    {
			    auto tmp = time_point_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const date*>)
		    {
			    auto tmp = date_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const time_of_day*>)
		    {
			    auto tmp = time_of_day_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    parameter.pointer());
}

void bind_parameters(sqlite3& connection, sqlite3_stmt& statement, const std::map<std::string, parameter>& parameters)
{
	for (const auto& pair : parameters)
	{
		bind_parameter(connection, statement, pair.first, pair.second);
	}
}

void store_string(sqlite3& connection, sqlite3_stmt& statement, int column, std::string_view column_name, std::string& out)
{
	const auto ptr = sqlite3_column_text(&statement, column);
	const auto len = sqlite3_column_bytes(&statement, column);
	if (!ptr)
	{
		std::ostringstream msg;
		msg << "sqlite3_column_text returned NULL for column " << std::quoted(column_name);
		throw error{ msg.str(), connection };
	}
	else if (len < 0)
	{
		std::ostringstream msg;
		msg << "sqlite3_column_bytes returned " << len << " for column " << std::quoted(column_name);
		throw error{ msg.str(), connection };
	}
	out.assign(reinterpret_cast<const char*>(ptr), len);
}

void store_result(sqlite3&                         connection,
                  sqlite3_stmt&                    statement,
                  const result::non_nullable_type& result,
                  int                              column,
                  std::string_view                 column_name,
                  int                              column_type)
{
	assert(SQLITE_NULL != column_type);
	(void)column_type;

	std::visit(
	    [&](auto&& arg) {
		    auto& destination = *arg;
		    using T           = std::decay_t<decltype(destination)>;
		    if constexpr (std::is_same_v<T, bool>)
		    {
			    destination = sqlite3_column_int(&statement, column) ? true : false;
		    }
		    else if constexpr (std::is_same_v<T, char>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column, column_name, tmp);
			    if (tmp.length() != 1)
			    {
				    std::ostringstream msg;
				    msg << "Cannot store the text value " << std::quoted(tmp) << " of column " << std::quoted(column_name)
				        << " in destination of type 'char' because the length is not 1";
				    throw error{ msg.str() };
			    }
			    else
			    {
				    destination = tmp.front();
			    }
		    }
		    else if constexpr (std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, std::int16_t> ||
		                       std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::int32_t>)
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
			    store_string(connection, statement, column, column_name, destination);
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
				    std::ostringstream msg;
				    msg << "sqlite3_column_blob returned NULL and sqlite3_column_bytes returned " << len << " for column "
				        << std::quoted(column_name);
				    throw error{ msg.str(), connection };
			    }
			    else if (len < 0)
			    {
				    std::ostringstream msg;
				    msg << "sqlite3_column_bytes returned " << len << " for column " << std::quoted(column_name);
				    throw error{ msg.str(), connection };
			    }
			    destination.assign(reinterpret_cast<const unsigned char*>(ptr), len);
		    }
		    else if constexpr (std::is_same_v<T, time_point>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column, column_name, tmp);
			    string_to_time_point(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, date>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column, column_name, tmp);
			    string_to_date(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, time_of_day>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column, column_name, tmp);
			    string_to_time_of_day(tmp, destination);
		    }
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    result);
}

void store_result(sqlite3& connection, sqlite3_stmt& statement, const result& result, std::string_view column_name, int column)
{
	const auto& destination = result.value();

	const auto column_type = sqlite3_column_type(&statement, column);

	if (SQLITE_NULL == column_type)
	{
		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, result::non_nullable_type>)
			    {
				    std::ostringstream msg;
				    msg << "Cannot store a NULL value of column " << std::quoted(column_name) << " in a non-optional type";
				    throw error{ msg.str() };
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
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
			    if constexpr (std::is_same_v<T, result::non_nullable_type>)
			    {
				    store_result(connection, statement, arg, column, column_name, column_type);
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        using T = typename std::decay_t<decltype(*arg)>::value_type;
					        T tmp{};
					        store_result(connection, statement, result::non_nullable_type{ &tmp }, column, column_name, column_type);
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

void store_result(sqlite3& connection, sqlite3_stmt& statement, const result& result, int column)
{
	store_result(connection, statement, result, sqlite3_column_name(&statement, column), column);
}

void store_results(sqlite3& connection, sqlite3_stmt& statement, const std::vector<result>& results)
{
	const auto columns = sqlite3_column_count(&statement);

	if (static_cast<int>(results.size()) > columns)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a row with only " + std::to_string(columns) +
			         " column" + (columns == 1 ? "" : "s") };
	}

	int current_column = 0;
	for (const auto& result : results)
	{
		assert(current_column < columns);
		store_result(connection, statement, result, current_column++);
	}
}

void store_results(sqlite3& connection, sqlite3_stmt& statement, const std::map<std::string, result>& results)
{
	const auto columns = sqlite3_column_count(&statement);

	if (static_cast<int>(results.size()) > columns)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a row with only " + std::to_string(columns) +
			         " column" + (columns == 1 ? "" : "s") };
	}

	std::map<std::string_view, int> map{};
	for (int index = 0, end = sqlite3_column_count(&statement); index < end; ++index)
	{
		map[sqlite3_column_name(&statement, index)] = index;
	}

	for (const auto& result : results)
	{
		auto it = map.find(result.first);
		if (it == map.end())
		{
			throw error{ "Column '" + result.first + "' not found in the result" };
		}
		store_result(connection, statement, result.second, result.first, it->second);
	}
}

} // namespace

statement::statement(std::shared_ptr<sqlite3> connection, std::string_view query, bool reuse_statement)
    : ibackend_statement{}
    , connection_{ connection }
    , query_{ query }
    , reuse_statement_{ reuse_statement }
    , statement_{}
    , step_result_{ -1 }
{
	assert(this->connection_);
}

void statement::execute(const std::map<std::string, parameter>& parameters)
{
	assert(this->connection_);

	if (this->statement_)
	{
		if (this->reuse_statement_)
		{
			if (SQLITE_OK != sqlite3_reset(this->statement_.get()))
			{
				throw error{ "sqlite3_reset failed", *this->connection_ };
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

	this->step_result_ = sqlite3_step(this->statement_.get());

	if (SQLITE_DONE != this->step_result_ && SQLITE_ROW != this->step_result_)
	{
		this->statement_.reset();
		throw error{ "sqlite3_step failed", *this->connection_ };
	}
}

bool statement::fetch(const std::vector<result>& results)
{
	assert(this->connection_);

	if (!this->statement_)
	{
		throw error{ "Cannot fetch row from a statement that has not been executed" };
	}

	if (SQLITE_DONE == this->step_result_)
	{
		return false;
	}
	assert(SQLITE_ROW == this->step_result_);

	store_results(*this->connection_, *this->statement_, results);

	this->step_result_ = sqlite3_step(this->statement_.get());

	if (SQLITE_DONE != this->step_result_ && SQLITE_ROW != this->step_result_)
	{
		throw error{ "sqlite3_step failed", *this->connection_ };
	}

	return true;
}

bool statement::fetch(const std::map<std::string, result>& results)
{
	assert(this->connection_);

	if (!this->statement_)
	{
		throw error{ "Cannot fetch row from a statement that has not been executed" };
	}

	if (SQLITE_DONE == this->step_result_)
	{
		return false;
	}
	assert(SQLITE_ROW == this->step_result_);

	store_results(*this->connection_, *this->statement_, results);

	this->step_result_ = sqlite3_step(this->statement_.get());

	if (SQLITE_DONE != this->step_result_ && SQLITE_ROW != this->step_result_)
	{
		throw error{ "sqlite3_step failed", *this->connection_ };
	}

	return true;
}

std::size_t statement::field_count()
{
	assert(this->connection_);

	if (!this->statement_)
	{
		throw error{ "Cannot get field count from a statement that has not been executed" };
	}

	const auto n = sqlite3_column_count(this->statement_.get());
	if (n < 0)
	{
		throw error{ "sqlite3_column_count returned a negative value" };
	}

	return static_cast<std::size_t>(n);
}

std::string statement::field_name(std::size_t index)
{
	assert(this->connection_);

	if (!this->statement_)
	{
		throw error{ "Cannot get field count from a statement that has not been executed" };
	}

	const auto name = sqlite3_column_name(this->statement_.get(), static_cast<int>(index));
	if (name == nullptr)
	{
		throw error{ "sqlite3_column_name returned a null pointer" };
	}

	return name;
}

void statement::execute(sqlite3& connection, const std::string& query)
{
	std::shared_ptr<sqlite3_stmt> statement{ prepare_statement(connection, query), sqlite3_finalize };

	auto rc = sqlite3_step(statement.get());
	if (SQLITE_DONE != rc && SQLITE_ROW != rc)
	{
		throw error{ "sqlite3_step failed", connection };
	}
}

} // namespace sqlite
} // namespace squid
