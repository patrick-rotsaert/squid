//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlqueryparameters.h"
#include "postgresqlquery.h"
#include "pgconversions.h"

#include "squid/postgresql/postgresqlerror.h"

#include "squid/detail/always_false.h"
#include "squid/detail/conversions.h"

#include <cassert>
#include <sstream>
#include <iomanip>
#include <type_traits>

namespace squid {
namespace postgresql {

namespace {

const char* get_parameter_value(const Parameter& parameter, std::string& value)
{
	if (std::holds_alternative<std::nullopt_t>(parameter.value()))
	{
		return nullptr;
	}

	std::visit(
	    [&value](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, std::nullopt_t>)
		    {
			    assert(false && "should not happen, this alternative was already tested");
		    }
		    else if constexpr (std::is_same_v<T, bool>)
		    {
			    value = arg ? "t" : "f";
		    }
		    else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> ||
		                       std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::int32_t> ||
		                       std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>)
		    {
			    value = std::to_string(arg);
		    }
		    else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>)
		    {
			    std::ostringstream ss;
			    ss << std::setprecision(std::numeric_limits<T>::digits10) << arg;
			    value = ss.str();
		    }
		    else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
		    {
			    value = arg;
		    }
		    else if constexpr (std::is_same_v<T, byte_string> || std::is_same_v<T, byte_string_view>)
		    {
			    binary_to_pg_hex_string(arg.begin(), arg.end(), value);
		    }
		    else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>)
		    {
			    time_point_to_string(arg, value);
		    }
		    else if constexpr (std::is_same_v<T, std::chrono::year_month_day>)
		    {
			    year_month_day_to_string(arg, value);
		    }
		    else if constexpr (std::is_same_v<T, std::chrono::hh_mm_ss<std::chrono::microseconds>>)
		    {
			    hh_mm_ss_to_string(arg, value);
		    }
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    parameter.value());

	return value.c_str();
}

} // namespace

PostgresqlQueryParameters::PostgresqlQueryParameters(const PostgresqlQuery& query, const std::map<std::string, Parameter>& parameters)
    : parameterValues{ static_cast<size_t>(query.nParams()) }
    , parameterValuePointers{ static_cast<size_t>(query.nParams()) }
{
	for (const auto& pair : query.parameterNamePosMap())
	{
		auto it = parameters.find(pair.first);
		if (it == parameters.end())
		{
			throw PostgresqlError{ "The query parameter '" + pair.first + "' is not bound" };
		}
		const auto& parameter = it->second;

		const auto& position = pair.second;
		assert(position >= 1 && position <= static_cast<decltype(position)>(this->parameterValues.size()));
		const auto index = position - 1;

		this->parameterValuePointers.at(index) = get_parameter_value(parameter, this->parameterValues.at(index));
	}
}

const char* const* PostgresqlQueryParameters::paramValues() const
{
	return &this->parameterValuePointers.at(0);
}

int PostgresqlQueryParameters::nParams() const
{
	return static_cast<int>(this->parameterValuePointers.size());
}

} // namespace postgresql
} // namespace squid
