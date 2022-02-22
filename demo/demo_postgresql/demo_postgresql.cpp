//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/detail/conversions.h"

#include "squid/connection.h"
#include "squid/statement.h"
#include "squid/preparedstatement.h"

#include "squid/postgresql/postgresqlconnection.h"

#include <string>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace squid {

// Statement foo(Connection& connection)
// {
// 	Statement st{ connection, "SELECT 42" };
// 	return st;
// }

enum class MyCharEnum : char
{
	FIRST  = 'a',
	SECOND = 'b',
	THIRD  = 'c',
};

enum class MyIntEnum : int
{
	FIRST  = 42,
	SECOND = 43,
};

void test()
{
	PostgresqlConnection connection{ "host=localhost port=54321 dbname=squid_demo_postgresql user=postgres password=Pass123" };

	{
		std::optional<double> optdouble = std::nullopt;
		unsigned char         bytes[]   = { 0xDE, 0xAD, 0xBE, 0xEF };
		byte_string           bytes2{ bytes, sizeof(bytes) };
		std::string           s{ "foo" };

		//auto st = foo(connection);
		Statement st{
			connection,
			"SELECT"
			"  :a AS a"
			", :b AS b"
			", :c AS c"
			", :d AS d"
			", :e AS e"
			", current_timestamp as tp"
			", current_time as tm"
			", :f AS f"
			", :g AS g"
			", :h AS h"
			", :i AS i"
			", :j AS j"
			", :k AS k"
			", :l AS l"
			", :m AS m"
			", :n AS n"
			", :o AS o"
			", :p AS p"
			", :q AS q"
			", :r AS r"
			"" //
		};

		st.bind("a", 123).bind("b", 42.23);
		st.bind("c", "bar");
		st.bind("d", std::string{ "baz" });
		st.bind("e", optdouble);
		st.bind("f", std::string_view{ "bar" });
		optdouble = 3.14159265359;
		st.bind("g", optdouble);
		st.bind("h", std::nullopt);
		st.bind("i", MyCharEnum::SECOND);
		st.bind("j", MyIntEnum::SECOND);
		st.bind("k", false);
		st.bind("l", byte_string_view{ bytes, sizeof(bytes) });
		st.bind("m", bytes2);
		st.bind("n", bytes, sizeof(bytes));
		st.bind("o", std::chrono::system_clock::now());
		st.bind("p", s);
		st.bind("q", std::string_view{ s });
		st.bind("r", std::chrono::year_month_day{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) });

		char                                  a = -1;
		float                                 b = -1;
		std::string                           c, d;
		std::optional<double>                 e;
		std::chrono::system_clock::time_point tp;

		st.bindResult(a).bindResult(b).bindResult(c).bindResult(d).bindResult(e).bindResult(tp);

		st.execute();
		while (st.fetch())
		{
			std::cout << "a=" << static_cast<int>(a) << ", b=" << b << ", c=" << std::quoted(c) << ", d=" << std::quoted(d)
			          << ", e=" << (e ? std::to_string(e.value()) : "<NULL>") << ", tp=" << time_point_to_string(tp) << "Z"
			          << "\n";
		}
	}
}

} // namespace squid

int main()
{
	try
	{
		squid::test();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}
}
