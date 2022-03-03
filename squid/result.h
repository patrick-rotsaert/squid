//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/types.h"
#include "squid/detail/is_optional.h"

#include <variant>
#include <cstdint>
#include <string>
#include <optional>

namespace squid {

/// This class holds a pointer to a bound result column.
class SQUID_EXPORT Result
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
	                                             time_point*,
	                                             date*,
	                                             time_of_day*>;
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
                                             std::optional<time_point>*,
                                             std::optional<date>*,
                                             std::optional<time_of_day>*>;
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

	/// Get the value
	const value_type& value() const noexcept;

private:
	value_type value_;
};

} // namespace squid
