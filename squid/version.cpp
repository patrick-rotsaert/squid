//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "version.h"

#include <cassert>

namespace squid {

int Version::number()
{
	return SQUID_VERSION_NUMBER;
}

int Version::major()
{
	return SQUID_VERSION_MAJOR;
}

int Version::minor()
{
	return SQUID_VERSION_MINOR;
}

int Version::patch()
{
	return SQUID_VERSION_PATCH;
}

void Version::check()
{
	assert(SQUID_VERSION_NUMBER == number());
}

} // namespace squid
