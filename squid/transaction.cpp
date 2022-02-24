#include "transaction.h"
#include "connection.h"

namespace squid {

Transaction::Transaction(Connection& connection)
    : connection_{ connection }
    , finished_{}
{
	connection.execute("BEGIN");
}

Transaction::~Transaction() noexcept
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

void Transaction::commit()
{
	if (!this->finished_)
	{
		// If the commit fails, the destructor must not do a ROLLBACK,
		// so this->finished_ is set before doing the COMMIT.
		this->finished_ = true;
		this->connection_.execute("COMMIT");
	}
}

void Transaction::rollback()
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
