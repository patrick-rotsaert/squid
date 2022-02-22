//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "api.h"
#include "byte_string.h"
#include "detail/is_optional.h"

#include <variant>
#include <cstdint>
#include <string>
#include <optional>
#include <chrono>

namespace squid {

class SQUID_API Result
{
public:
	using non_nullable_value_type = std::variant<bool*,
	                                             char*,
	                                             signed char*,
	                                             unsigned char*,
	                                             std::int16_t*,
	                                             std::uint16_t*,
	                                             std::int32_t*,
	                                             std::uint32_t*,
	                                             std::int64_t*,
	                                             std::uint64_t*,
	                                             float*,
	                                             double*,
	                                             long double*,
	                                             std::string*,
	                                             byte_string*,
	                                             std::chrono::system_clock::time_point*,
	                                             std::chrono::year_month_day*,
	                                             std::chrono::hh_mm_ss<std::chrono::microseconds>*>;
	using nullable_value_type     = std::variant<std::optional<bool>*,
                                             std::optional<char>*,
                                             std::optional<signed char>*,
                                             std::optional<unsigned char>*,
                                             std::optional<std::int16_t>*,
                                             std::optional<std::uint16_t>*,
                                             std::optional<std::int32_t>*,
                                             std::optional<std::uint32_t>*,
                                             std::optional<std::int64_t>*,
                                             std::optional<std::uint64_t>*,
                                             std::optional<float>*,
                                             std::optional<double>*,
                                             std::optional<long double>*,
                                             std::optional<std::string>*,
                                             std::optional<byte_string>*,
                                             std::optional<std::chrono::system_clock::time_point>*,
                                             std::optional<std::chrono::year_month_day>*,
                                             std::optional<std::chrono::hh_mm_ss<std::chrono::microseconds>>*>;
	using value_type              = std::variant<non_nullable_value_type, nullable_value_type>;

	template<typename T>
	explicit Result(T& value)
	    : value_{}
	{
		// TODO: handle enum types
		if constexpr (is_optional_v<T>)
		{
			nullable_value_type tmp = &value;
			this->value_            = tmp;
		}
		else
		{
			non_nullable_value_type tmp = &value;
			this->value_                = tmp;
		}
	}

	Result(const Result&) = delete;
	Result(Result&& src)  = default;
	Result& operator=(const Result&) = delete;
	Result& operator=(Result&&) = default;

	const value_type& value() const noexcept;

private:
	value_type value_;
};

} // namespace squid
