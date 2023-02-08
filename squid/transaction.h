//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"

namespace squid {

class connection;

class SQUID_EXPORT transaction final
{
	connection& connection_;
	bool        finished_;

public:
	/// Start a database transaction
	explicit transaction(connection& connection);

	/// The destructor will rollback the transaction if neither commit() nor rollback() was called
	~transaction() noexcept;

	transaction(const transaction&)            = delete;
	transaction(transaction&& src)             = delete;
	transaction& operator=(const transaction&) = delete;
	transaction& operator=(transaction&&)      = delete;

	void commit();
	void rollback();
};

} // namespace squid
