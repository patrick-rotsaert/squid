//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "demo/demo_common/demo_common.h"

#include "squid/sqlite3/connection.h"

#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace squid {
namespace demo {

void demo()
{
	const auto path = (std::filesystem::temp_directory_path() / "demo_sqlite3.db").string();
	// const auto path = ":memory:";

	sqlite::connection connection{ path };
	std::cout << "opened database " << std::quoted(path) << "\n";

	demo_all(connection, Backend::SQLITE3);
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
