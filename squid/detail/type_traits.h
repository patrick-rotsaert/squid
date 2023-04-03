//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <utility>

namespace squid {

template<class T, class Binder>
concept has_bind_method = requires(T& t)
{
	t.bind(std::declval<Binder&>());
};

template<typename...>
using void_t = void;

template<typename Type, typename Archive, typename = void_t<>>
struct is_boost_serializable : std::false_type
{
};

template<typename Type, typename Archive>
struct is_boost_serializable<Type, Archive, void_t<decltype(std::declval<Type&>().serialize(std::declval<Archive&>(), 0))>> : std::true_type
{
};

template<typename Type, typename Archive>
inline constexpr bool is_boost_serializable_v = is_boost_serializable<Type, Archive>::value;

template<typename Type, typename Archive, typename = void_t<>>
struct nested_is_boost_serializable : is_boost_serializable<Type, Archive>
{
};

template<typename Type, typename Archive>
struct nested_is_boost_serializable<Type, Archive, void_t<typename Type::value_type>>
    : nested_is_boost_serializable<typename Type::value_type, Archive>
{
};

} // namespace squid
