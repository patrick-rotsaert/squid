//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <type_traits>

#if __cplusplus < 202100
namespace std {

template<typename T, bool B = std::is_enum_v<T>>
struct is_scoped_enum : std::false_type
{
};

template<typename T>
struct is_scoped_enum<T, true> : std::integral_constant<bool, !std::is_convertible_v<T, std::underlying_type_t<T>>>
{
};

template<typename T>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

} // namespace std
#endif
