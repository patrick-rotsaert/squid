//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "queryresults.h"

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
                  int                              column_index,
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
			    destination = sqlite3_column_int(&statement, column_index) ? true : false;
		    }
		    else if constexpr (std::is_same_v<T, char>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column_index, column_name, tmp);
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
			    destination = static_cast<T>(sqlite3_column_int(&statement, column_index));
		    }
		    else if constexpr (std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>)
		    {
			    destination = static_cast<T>(sqlite3_column_int64(&statement, column_index));
		    }
		    else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>)
		    {
			    destination = static_cast<T>(sqlite3_column_double(&statement, column_index));
		    }
		    else if constexpr (std::is_same_v<T, std::string>)
		    {
			    store_string(connection, statement, column_index, column_name, destination);
		    }
		    else if constexpr (std::is_same_v<T, byte_string>)
		    {
			    const auto ptr = sqlite3_column_blob(&statement, column_index);
			    const auto len = sqlite3_column_bytes(&statement, column_index);
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
			    store_string(connection, statement, column_index, column_name, tmp);
			    string_to_time_point(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, date>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column_index, column_name, tmp);
			    string_to_date(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, time_of_day>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column_index, column_name, tmp);
			    string_to_time_of_day(tmp, destination);
		    }
#ifdef SQUID_HAVE_BOOST_DATE_TIME
		    else if constexpr (std::is_same_v<T, boost::posix_time::ptime>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column_index, column_name, tmp);
			    string_to_boost_ptime(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, boost::gregorian::date>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column_index, column_name, tmp);
			    string_to_boost_date(tmp, destination);
		    }
		    else if constexpr (std::is_same_v<T, boost::posix_time::time_duration>)
		    {
			    std::string tmp;
			    store_string(connection, statement, column_index, column_name, tmp);
			    string_to_boost_time_duration(tmp, destination);
		    }
#endif
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    result);
}

void store_result(sqlite3&         connection,
                  sqlite3_stmt&    statement,
                  const result&    result,
                  int              column_index,
                  std::string_view column_name,
                  int              column_type)
{
	const auto& destination = result.value();

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
					        arg->reset();
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
				    store_result(connection, statement, arg, column_index, column_name, column_type);
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        using T = typename std::decay_t<decltype(*arg)>::value_type;
					        T tmp{};
					        store_result(connection, statement, result::non_nullable_type{ &tmp }, column_index, column_name, column_type);
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

} // namespace

struct query_results::column
{
	result           res;
	std::string_view name;
	int              type;
	int              index;

	column(const result& res, std::string_view name, int type, int index)
	    : res{ res }
	    , name{ std::move(name) }
	    , type{ type }
	    , index{ index }
	{
	}
};

query_results::query_results(std::shared_ptr<sqlite3> connection, std::shared_ptr<sqlite3_stmt> statement)
    : connection_{ std::move(connection) }
    , statement_{ std::move(statement) }
    , columns_{}
    , field_count_{}
{
	assert(this->connection_);
	assert(this->statement_);

	const auto column_count = sqlite3_column_count(this->statement_.get());
	if (column_count < 0)
	{
		throw error{ "sqlite3_column_count returned a negative value" };
	}

	this->field_count_ = static_cast<size_t>(column_count);
}

query_results::query_results(std::shared_ptr<sqlite3>      connection,
                             std::shared_ptr<sqlite3_stmt> statement,
                             const std::vector<result>&    results)
    : query_results{ connection, statement }
{
	if (results.size() > this->field_count_)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a row with only " +
			         std::to_string(this->field_count_) + " column" + (this->field_count_ == 1 ? "" : "s") };
	}

	this->columns_.reserve(results.size());
	int index = 0;
	for (const auto& result : results)
	{
		const auto column_name = sqlite3_column_name(statement.get(), index);
		if (column_name == nullptr)
		{
			throw error{ "sqlite3_column_name returned a nullptr" };
		}

		this->columns_.push_back(std::make_unique<column>(result, column_name, sqlite3_column_type(statement.get(), index), index));

		++index;
	}
}

query_results::query_results(std::shared_ptr<sqlite3>             connection,
                             std::shared_ptr<sqlite3_stmt>        statement,
                             const std::map<std::string, result>& results)
    : query_results{ connection, statement }
{
	if (results.size() > this->field_count_)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a row with only " +
			         std::to_string(this->field_count_) + " column" + (this->field_count_ == 1 ? "" : "s") };
	}

	std::map<std::string_view, size_t> map{};
	for (size_t index = 0; index < this->field_count_; ++index)
	{
		const auto column_name = sqlite3_column_name(statement.get(), index);
		if (column_name == nullptr)
		{
			throw error{ "sqlite3_column_name returned a nullptr" };
		}
		map[column_name] = index;
	}

	this->columns_.reserve(results.size());
	for (const auto& result : results)
	{
		auto it = map.find(result.first);
		if (it == map.end())
		{
			throw error{ "Column '" + result.first + "' not found in the result" };
		}

		const auto index       = it->second;
		const auto column_name = it->first;

		this->columns_.push_back(std::make_unique<column>(result.second, column_name, sqlite3_column_type(statement.get(), index), index));
	}
}

query_results::~query_results() noexcept
{
}

size_t query_results::field_count() const
{
	return this->field_count_;
}

std::string query_results::field_name(std::size_t index) const
{
	const auto name = sqlite3_column_name(this->statement_.get(), static_cast<int>(index));

	if (name == nullptr)
	{
		throw error{ "sqlite3_column_name returned a null pointer" };
	}

	return name;
}

void query_results::fetch()
{
	for (const auto& column : this->columns_)
	{
		store_result(*this->connection_, *this->statement_, column->res, column->index, column->name, column->type);
	}
}

} // namespace sqlite
} // namespace squid
