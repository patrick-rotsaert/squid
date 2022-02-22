//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <type_traits>
#include <optional>

namespace squid {

template<typename T, typename Enable = void>
struct is_optional : std::false_type
{
};

template<typename T>
struct is_optional<std::optional<T>> : std::true_type
{
};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

} // namespace squid
