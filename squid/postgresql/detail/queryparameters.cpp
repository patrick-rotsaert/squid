//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/detail/queryparameters.h"
#include "squid/postgresql/detail/query.h"
#include "squid/postgresql/detail/conversions.h"
#include "squid/postgresql/error.h"

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
	const auto pointer = parameter.pointer();

	if (std::holds_alternative<const std::nullopt_t*>(pointer))
	{
		return nullptr;
	}

	std::visit(
	    [&value](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, const std::nullopt_t*>)
		    {
			    assert(false && "should not happen, this alternative was already tested");
		    }
		    else if constexpr (std::is_same_v<T, const bool*>)
		    {
			    assert(arg != nullptr);
			    value = *arg ? "t" : "f";
		    }
		    else if constexpr (std::is_same_v<T, const char*>)
		    {
			    assert(arg != nullptr);
			    value = std::string{ arg, 1 };
		    }
		    else if constexpr (std::is_same_v<T, const signed char*> || std::is_same_v<T, const unsigned char*> ||
		                       std::is_same_v<T, const std::int16_t*> || std::is_same_v<T, const std::uint16_t*> ||
		                       std::is_same_v<T, const std::int32_t*> || std::is_same_v<T, const std::uint32_t*> ||
		                       std::is_same_v<T, const std::int64_t*> || std::is_same_v<T, const std::uint64_t*>)
		    {
			    assert(arg != nullptr);
			    value = std::to_string(*arg);
		    }
		    else if constexpr (std::is_same_v<T, const float*> || std::is_same_v<T, const double*> || std::is_same_v<T, const long double*>)
		    {
			    assert(arg != nullptr);
			    std::ostringstream ss;
			    ss << std::setprecision(std::numeric_limits<T>::digits10) << *arg;
			    value = ss.str();
		    }
		    else if constexpr (std::is_same_v<T, const std::string*> || std::is_same_v<T, const std::string_view*>)
		    {
			    assert(arg != nullptr);
			    value = *arg;
		    }
		    else if constexpr (std::is_same_v<T, const byte_string*> || std::is_same_v<T, const byte_string_view*>)
		    {
			    assert(arg != nullptr);
			    binary_to_hex_string(arg->begin(), arg->end(), value);
		    }
		    else if constexpr (std::is_same_v<T, const time_point*>)
		    {
			    assert(arg != nullptr);
			    time_point_to_string(*arg, value);
		    }
		    else if constexpr (std::is_same_v<T, const date*>)
		    {
			    assert(arg != nullptr);
			    year_month_day_to_string(*arg, value);
		    }
		    else if constexpr (std::is_same_v<T, const time_of_day*>)
		    {
			    assert(arg != nullptr);
			    hh_mm_ss_to_string(*arg, value);
		    }
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    pointer);

	return value.c_str();
}

} // namespace

QueryParameters::QueryParameters(const Query& query, const std::map<std::string, Parameter>& parameters)
    : parameterValues{ static_cast<size_t>(query.nParams()) }
    , parameterValuePointers{ static_cast<size_t>(query.nParams()) }
{
	for (const auto& pair : query.parameterNamePosMap())
	{
		auto it = parameters.find(pair.first);
		if (it == parameters.end())
		{
			throw Error{ "The query parameter '" + pair.first + "' is not bound" };
		}
		const auto& parameter = it->second;

		const auto& position = pair.second;
		assert(position >= 1 && position <= static_cast<decltype(position)>(this->parameterValues.size()));
		const auto index = position - 1;

		this->parameterValuePointers.at(index) = get_parameter_value(parameter, this->parameterValues.at(index));
	}
}

const char* const* QueryParameters::paramValues() const
{
	return &this->parameterValuePointers.at(0);
}

int QueryParameters::nParams() const
{
	return static_cast<int>(this->parameterValuePointers.size());
}

} // namespace postgresql
} // namespace squid
