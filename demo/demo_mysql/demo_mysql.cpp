//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "demo/demo_common/demo_common.h"

#include "squid/mysql/connection.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace squid {
namespace demo {

void demo()
{
	constexpr auto connection_info = "host=127.0.0.1 port=13306 db=squid_demo_mysql user=myuser passwd=Pass123";

	mysql::connection connection{ connection_info };
	std::cout << "opened database " << std::quoted(connection_info) << "\n";

	demo_all(connection, Backend::MYSQL);
}

} // namespace demo
} // namespace squid

int main()
{
	try
	{
		squid::demo::demo();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}
}
