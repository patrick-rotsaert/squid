//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/ibackendconnectionfactory.h"

namespace squid {
namespace sqlite {

class isqlite_api;

class SQUID_EXPORT backend_connection_factory final : public ibackend_connection_factory
{
	isqlite_api* api_;

public:
	explicit backend_connection_factory(isqlite_api& api);

	backend_connection_factory(const backend_connection_factory&)            = delete;
	backend_connection_factory(backend_connection_factory&& src)             = default;
	backend_connection_factory& operator=(const backend_connection_factory&) = delete;
	backend_connection_factory& operator=(backend_connection_factory&&)      = default;

	std::shared_ptr<ibackend_connection> create_backend_connection(std::string_view connection_info) const override;
};

} // namespace sqlite
} // namespace squid
