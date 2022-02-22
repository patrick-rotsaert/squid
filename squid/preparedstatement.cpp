//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "preparedstatement.h"
#include "connection.h"
#include "ibackendconnection.h"
#include "ibackendstatement.h"

namespace squid {

PreparedStatement::PreparedStatement(Connection& connection, std::string_view query)
    : BasicStatement{ connection.backend()->createPreparedStatement(query) }
{
}

} // namespace squid
