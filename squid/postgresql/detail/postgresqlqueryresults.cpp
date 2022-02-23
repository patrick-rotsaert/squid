//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlqueryresults.h"
#include "pgconversions.h"

#include "squid/postgresql/postgresqlerror.h"

#include "squid/detail/conversions.h"
#include "squid/detail/always_false.h"
#include "squid/detail/demangled_type_name.h"

#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace squid {
namespace postgresql {

namespace {

void store_result(const Result::non_nullable_value_type& result, std::string_view fieldName, std::string_view value)
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
			    else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> ||
			                       std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::int32_t> ||
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
			    else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>)
			    {
				    string_to_time_point(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, std::chrono::year_month_day>)
			    {
				    string_to_year_month_day(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, std::chrono::hh_mm_ss<std::chrono::microseconds>>)
			    {
				    string_to_hh_mm_ss(value, destination);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    }
		    catch (const std::exception& e)
		    {
			    std::ostringstream error;
			    error << "Cannot convert the text value " << std::quoted(value) << " of field " << std::quoted(fieldName)
			          << " to destination type " << demangled_type_name<T>() << ": " << e.what();
			    throw Error{ error.str() };
		    }
	    },
	    result);
}

void store_result(const Result& result, const PGresult& pgResult, int row, int column)
{
	assert(row < PQntuples(&pgResult));
	assert(column < PQnfields(&pgResult));

	const auto& destination = result.value();

	const auto fieldName = PQfname(&pgResult, column);
	assert(fieldName);

	if (PQgetisnull(&pgResult, row, column))
	{
		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, Result::non_nullable_value_type>)
			    {
				    std::ostringstream error;
				    error << "Cannot store a NULL value of field " << std::quoted(fieldName) << " in a non-optional type";
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
		const auto value = PQgetvalue(&pgResult, row, column);
		assert(value);

		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, Result::non_nullable_value_type>)
			    {
				    store_result(arg, fieldName, value);
			    }
			    else if constexpr (std::is_same_v<T, Result::nullable_value_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        using T = typename std::decay_t<decltype(*arg)>::value_type;
					        T tmp{};
					        store_result(Result::non_nullable_value_type{ &tmp }, fieldName, value);
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

void QueryResults::store(const std::vector<Result>& results, const PGresult& pgResult, int row)
{
	const auto columns = PQnfields(&pgResult);

	if (static_cast<int>(results.size()) > columns)
	{
		throw Error{ "Cannot fetch " + std::to_string(results.size()) + " fields from a tuple with only " + std::to_string(columns) +
			         " field" + (columns == 1 ? "" : "s") };
	}

	int currentColumn = 0;
	for (const auto& result : results)
	{
		assert(currentColumn < columns);
		store_result(result, pgResult, row, currentColumn++);
	}
}

} // namespace postgresql
} // namespace squid
