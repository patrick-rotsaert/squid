//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <utility>

namespace squid {

template<typename T, typename Binder>
concept has_bind_method = requires(T& t)
{
	t.bind(std::declval<Binder&>());
};

} // namespace squid
