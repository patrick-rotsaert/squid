//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"

namespace squid {

class Connection;

class SQUID_EXPORT Transaction final
{
	Connection& connection_;
	bool        finished_;

public:
	/// Start a database transaction
	explicit Transaction(Connection& connection);

	/// The destructor will rollback the transaction if neither commit() nor rollback() was called
	~Transaction() noexcept;

	Transaction(const Transaction&)            = delete;
	Transaction(Transaction&& src)             = delete;
	Transaction& operator=(const Transaction&) = delete;
	Transaction& operator=(Transaction&&)      = delete;

	void commit();
	void rollback();
};

} // namespace squid
