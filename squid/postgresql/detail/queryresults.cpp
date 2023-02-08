//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/detail/queryresults.h"
#include "squid/postgresql/detail/conversions.h"

#include "squid/postgresql/error.h"

#include "squid/detail/conversions.h"
#include "squid/detail/always_false.h"
#include "squid/detail/demangled_type_name.h"

#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iomanip>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

namespace {

void store_result(const result::non_nullable_type& result, std::string_view column_name, std::string_view value)
{
	std::visit(
	    [&](auto&& arg) {
		    auto& destination = *arg;
		    using T           = std::decay_t<decltype(destination)>;
		    try
		    {
			    if constexpr (std::is_same_v<T, bool>)
			    {
				    if (value == "t")
				    {
					    destination = true;
				    }
				    else if (value == "f")
				    {
					    destination = false;
				    }
				    else
				    {
					    throw std::runtime_error{ "value not 't' nor 'f'" };
				    }
			    }
			    else if constexpr (std::is_same_v<T, char>)
			    {
				    if (value.length() != 1)
				    {
					    throw std::runtime_error{ "length is not 1" };
				    }
				    else
				    {
					    destination = value.front();
				    }
			    }
			    else if constexpr (std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, std::int16_t> ||
			                       std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::int32_t> ||
			                       std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::int64_t> ||
			                       std::is_same_v<T, std::uint64_t> || std::is_same_v<T, float> || std::is_same_v<T, double> ||
			                       std::is_same_v<T, long double>)
			    {
				    string_to_number(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, std::string>)
			    {
				    destination = value;
			    }
			    else if constexpr (std::is_same_v<T, byte_string>)
			    {
				    hex_string_to_binary(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, time_point>)
			    {
				    string_to_time_point(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, date>)
			    {
				    string_to_date(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, time_of_day>)
			    {
				    string_to_time_of_day(value, destination);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    }
		    catch (const std::exception& e)
		    {
			    std::ostringstream msg;
			    msg << "Cannot convert the text value " << std::quoted(value) << " of column " << std::quoted(column_name)
			        << " to destination type " << demangled_type_name<T>() << ": " << e.what();
			    throw error{ msg.str() };
		    }
	    },
	    result);
}

void store_result(const result& result, const PGresult& pgresult, int row, std::string_view column_name, int column)
{
	assert(row < PQntuples(&pgresult));
	assert(column < PQnfields(&pgresult));
	assert(column_name.data());

	const auto& destination = result.value();

	if (PQgetisnull(&pgresult, row, column))
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
		const auto value = PQgetvalue(&pgresult, row, column);
		assert(value);

		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, result::non_nullable_type>)
			    {
				    store_result(arg, column_name, value);
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        using T = typename std::decay_t<decltype(*arg)>::value_type;
					        T tmp{};
					        store_result(result::non_nullable_type{ &tmp }, column_name, value);
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

void store_result(const result& result, const PGresult& pgresult, int row, int column)
{
	store_result(result, pgresult, row, PQfname(&pgresult, column), column);
}

void store_result(const result& result, const PGresult& pgresult, int row, const std::string& column_name)
{
	const auto column = PQfnumber(&pgresult, column_name.c_str());
	if (column < 0)
	{
		throw error{ "Column '" + column_name + "' not found in result" };
	}
	store_result(result, pgresult, row, column_name, column);
}

} // namespace

void query_results::store(const std::vector<result>& results, const PGresult& pgresult, int row)
{
	const auto columns = PQnfields(&pgresult);

	if (static_cast<int>(results.size()) > columns)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a tuple with only " + std::to_string(columns) +
			         " column" + (columns == 1 ? "" : "s") };
	}

	int current_column = 0;
	for (const auto& result : results)
	{
		assert(current_column < columns);
		store_result(result, pgresult, row, current_column++);
	}
}

void query_results::store(const std::map<std::string, result>& results, const PGresult& pgresult, int row)
{
	const auto columns = PQnfields(&pgresult);

	if (static_cast<int>(results.size()) > columns)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a tuple with only " + std::to_string(columns) +
			         " column" + (columns == 1 ? "" : "s") };
	}

	for (const auto& result : results)
	{
		store_result(result.second, pgresult, row, result.first);
	}
}

} // namespace postgresql
} // namespace squid
