//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/preparedstatement.h"
#include "squid/connection.h"
#include "squid/ibackendconnection.h"
#include "squid/ibackendstatement.h"

namespace squid {

std::unique_ptr<ibackend_statement> prepared_statement::create_statement(std::shared_ptr<ibackend_connection> connection,
                                                                         std::string_view                     query)
{
	return connection->create_prepared_statement(query);
}

prepared_statement::prepared_statement(connection& connection, std::string_view query)
    : basic_statement{ connection.backend(), connection.backend()->create_prepared_statement(query) }
{
}

prepared_statement::prepared_statement(connection& connection)
    : basic_statement{ connection.backend() }
{
}

} // namespace squid
