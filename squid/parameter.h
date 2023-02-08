//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/types.h"
#include "squid/error.h"
#include "squid/detail/is_optional.h"
#include "squid/detail/is_scoped_enum.h"

#include <variant>
#include <optional>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace squid {

/// This class holds a bound query parameter or a reference to it.
class SQUID_EXPORT parameter
{
public:
	using value_type = std::variant< //
	    std::nullopt_t,
	    bool,
	    char,
	    signed char,
	    unsigned char,
	    std::int16_t,
	    std::uint16_t,
	    std::int32_t,
	    std::uint32_t,
	    std::int64_t,
	    std::uint64_t,
	    float,
	    double,
	    long double,
	    std::string_view,
	    std::string,
	    byte_string_view,
	    byte_string,
	    time_point,
	    date,
	    time_of_day
	    //
	    >;

	using pointer_type = std::variant< //
	    const std::nullopt_t*,
	    const bool*,
	    const char*,
	    const signed char*,
	    const unsigned char*,
	    const std::int16_t*,
	    const std::uint16_t*,
	    const std::int32_t*,
	    const std::uint32_t*,
	    const std::int64_t*,
	    const std::uint64_t*,
	    const float*,
	    const double*,
	    const long double*,
	    const std::string_view*,
	    const std::string*,
	    const byte_string_view*,
	    const byte_string*,
	    const time_point*,
	    const date*,
	    const time_of_day*
	    //
	    >;

	using pointer_optional_type = std::variant< //
	    const std::optional<bool>*,
	    const std::optional<char>*,
	    const std::optional<signed char>*,
	    const std::optional<unsigned char>*,
	    const std::optional<std::int16_t>*,
	    const std::optional<std::uint16_t>*,
	    const std::optional<std::int32_t>*,
	    const std::optional<std::uint32_t>*,
	    const std::optional<std::int64_t>*,
	    const std::optional<std::uint64_t>*,
	    const std::optional<float>*,
	    const std::optional<double>*,
	    const std::optional<long double>*,
	    const std::optional<std::string_view>*,
	    const std::optional<std::string>*,
	    const std::optional<byte_string_view>*,
	    const std::optional<byte_string>*,
	    const std::optional<time_point>*,
	    const std::optional<date>*,
	    const std::optional<time_of_day>*
	    //
	    >;

	using reference_type = std::variant<pointer_type, pointer_optional_type>;

	using type = std::variant<value_type, reference_type>;

	// for tag dispatching
	struct by_value
	{
	};

	// for tag dispatching
	struct by_reference
	{
	};

	template<typename T>
	explicit parameter(const T& value, const by_value&)
	    : value_{ get_value(value) }
	{
	}

	template<typename T>
	explicit parameter(const T& value, const by_reference&)
	    : value_{ get_reference(value) }
	{
	}

	/// Holds a std::string_view, string content is not copied.
	/// @a value must be nul-terminated.
	explicit parameter(const char* value, const by_value&);

	parameter(const parameter&)            = delete;
	parameter(parameter&& src)             = default;
	parameter& operator=(const parameter&) = delete;
	parameter& operator=(parameter&&)      = default;

	/// Get the value pointer
	const pointer_type pointer() const;

	/// Get the value, used for unit tests only
	const type& value() const noexcept;

private:
	template<typename T>
	static value_type get_value(const T& value)
	{
		if constexpr (is_optional_v<T>)
		{
			if (value.has_value())
			{
				return parameter::get_value(value.value());
			}
			else
			{
				return std::nullopt;
			}
		}
		else if constexpr (std::is_enum_v<T>)
		{
			return static_cast<std::underlying_type_t<T>>(value);
		}
		else
		{
			return value;
		}
	}

	template<typename T>
	static reference_type get_reference(const T& value)
	{
		if constexpr (is_optional_v<T>)
		{
			using V = typename T::value_type;
			if constexpr (std::is_enum_v<V>)
			{
				return reinterpret_cast<const std::optional<std::underlying_type_t<V>>*>(&value);
			}
			else
			{
				return &value;
			}
		}
		else if constexpr (std::is_enum_v<T>)
		{
			return reinterpret_cast<const std::underlying_type_t<T>*>(&value);
		}
		else
		{
			return &value;
		}
	}

private:
	type value_;
};

} // namespace squid
