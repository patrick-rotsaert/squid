//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/detail/demangle.h"

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif

namespace squid {

#ifdef __GNUG__
std::string demangle(const char* name)
{
	int                                    status{};
	std::unique_ptr<char, void (*)(void*)> res{ abi::__cxa_demangle(name, NULL, NULL, &status), std::free };
	return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
std::string demangle(const char* name)
{
	return name;
}

#endif

} // namespace squid
