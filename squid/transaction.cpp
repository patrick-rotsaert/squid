//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "transaction.h"
#include "connection.h"

namespace squid {

transaction::transaction(connection& connection)
    : connection_{ connection }
    , finished_{}
{
	connection.execute("BEGIN");
}

transaction::~transaction() noexcept
{
	try
	{
		if (!this->finished_)
		{
			this->connection_.execute("ROLLBACK");
		}
	}
	catch (...)
	{
		;
	}
}

void transaction::commit()
{
	if (!this->finished_)
	{
		// If the commit fails, the destructor must not do a ROLLBACK,
		// so this->finished_ is set before doing the COMMIT.
		this->finished_ = true;
		this->connection_.execute("COMMIT");
	}
}

void transaction::rollback()
{
	if (!this->finished_)
	{
		// If the ROLLBACK fails, the destructor must not do a ROLLBACK again,
		// so this->finished_ is set before doing the ROLLBACK.
		this->finished_ = true;
		this->connection_.execute("ROLLBACK");
	}
}

} // namespace squid
