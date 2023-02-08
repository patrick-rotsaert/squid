//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/version.h"

#include <cassert>

namespace squid {

int version::number()
{
	return SQUID_VERSION_NUMBER;
}

int version::major()
{
	return SQUID_VERSION_MAJOR;
}

int version::minor()
{
	return SQUID_VERSION_MINOR;
}

int version::patch()
{
	return SQUID_VERSION_PATCH;
}

void version::check()
{
	assert(SQUID_VERSION_NUMBER == number());
}

} // namespace squid
