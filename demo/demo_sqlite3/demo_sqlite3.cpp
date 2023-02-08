//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/detail/conversions.h"

#include "squid/connection.h"
#include "squid/statement.h"
#include "squid/preparedstatement.h"
#include "squid/transaction.h"
#include "squid/config.h"

#include "squid/sqlite3/connection.h"

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
#include <boost/serialization/nvp.hpp>
#endif

#include <string>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <cassert>

namespace squid {

// statement foo(connection& connection)
// {
// 	statement st{ connection, "SELECT 42" };
// 	return st;
// }

enum MyEnum
{
	MyEnum_FIRST  = 42,
	MyEnum_SECOND = 43,
	MyEnum_THIRD  = 44,
};

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
	THIRD  = 44,
};

void test_field_info(connection& connection)
{
	statement st{ connection, "SELECT 42 AS first, 'foo' as second, 3.1415 as third" };
	st.execute();

	const auto fieldCount = st.field_count();
	std::cout << "field count: " << fieldCount << "\n";

	for (std::size_t i = 0; i < fieldCount; ++i)
	{
		std::cout << "field name [" << i << "]: " << std::quoted(st.field_name(i)) << "\n";
	}
}

void test_binding(connection& connection)
{
	std::optional<double> optdouble = std::nullopt;
	unsigned char         bytes[]   = { 0xDE, 0xAD, 0xBE, 0xEF };
	byte_string           bytes2{ bytes, sizeof(bytes) };
	std::string           s{ "foo" };

	//auto st = foo(connection);
	statement st{
		connection,
		"SELECT"
		"  :a AS a"
		", :b AS b"
		", :c AS c"
		", :d AS d"
		", :e AS e"
		", current_timestamp as tp"
		", :tp as tp2"
		", current_time as tm"
		", :f AS f"
		", :g AS g"
		", :h AS h"
		", :i AS i"
		", :j AS j"
		", :jj AS jj"
		", :k AS k"
		", :l AS l"
		", :m AS m"
		", :n AS n"
		", :o AS o"
		", :p AS p"
		", @q AS q"
		", $r AS r"
		"" //
	};

	st.bind("a", 'A').bind("b", 42.23);
	st.bind("c", "bar");
	st.bind("d", std::string{ "baz" });
	st.bind("e", optdouble);
	st.bind("f", std::string_view{ "bar" });
	optdouble = 3.14159265359;
	st.bind("g", optdouble);
	st.bind("h", std::nullopt);
	st.bind("i", MyCharEnum::SECOND);
	st.bind("j", MyIntEnum::SECOND);
	st.bind("jj", MyEnum_SECOND);
	st.bind("k", false);
	st.bind("l", byte_string_view{ bytes, sizeof(bytes) });
	st.bind("m", bytes2);
	st.bind("n", bytes, sizeof(bytes));
	st.bind("o", std::chrono::system_clock::now());
	st.bind("p", s);
	st.bind("q", std::string_view{ s });
	st.bind("r", date{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) });
	st.bind("tp", std::chrono::system_clock::now());

	char                  a = -1;
	float                 b = -1;
	std::string           c, d, f;
	std::optional<double> e, g, h;
	time_point            tp, tp2;
	time_of_day           tm;
	MyCharEnum            i;
	MyIntEnum             j;
	MyEnum                jj;

	// Old style: bind all result columns with individual bind_result calls.
	//	st.bind_result(a)
	//	    .bind_result(b)
	//	    .bind_result(c)
	//	    .bind_result(d)
	//	    .bind_result(e)
	//	    .bind_result(tp)
	//	    .bind_result(tp2)
	//	    .bind_result(tm)
	//	    .bind_result(f)
	//	    .bind_result(g)
	//	    .bind_result(h)
	//	    .bind_result(i)
	//	    .bind_result(j)
	//	    .bind_result(jj);

	// New style: bind all result columns with a single call.
	st.bind_results(a, b, c, d, e, tp, tp2, tm, f, g, h, i, j, jj);

	st.execute();

	auto fetched = st.fetch();
	assert(fetched);

	std::cout << "a=" << a << ", b=" << b << ", c=" << std::quoted(c) << ", d=" << std::quoted(d)
	          << ", e=" << (e ? std::to_string(e.value()) : "<NULL>") << ", tp=" << time_point_to_string(tp) << "Z"
	          << ", tp2=" << time_point_to_string(tp2) << "Z"
	          << "\n";

	assert(i == MyCharEnum::SECOND);
	assert(j == MyIntEnum::SECOND);
	assert(jj == MyEnum_SECOND);

	fetched = st.fetch();
	assert(!fetched);
}

void test_table_ops(connection& connection)
{
	// Drop all tables
	{
		transaction tr{ connection };

		std::vector<std::string> names;
		{
			statement   st{ connection, "SELECT name FROM sqlite_master WHERE type = 'table'" };
			std::string name;
			st.bind_result(name);
			st.execute();
			while (st.fetch())
			{
				names.push_back(name);
			}
		}

		{
			for (const auto& name : names)
			{
				connection.execute("DROP TABLE IF EXISTS \"" + name + "\"");
			}
		}

		tr.commit();
	}

	{
		constexpr auto query =
		    "CREATE TABLE guitar ("
		    "  guitar_id  INTEGER PRIMARY KEY"
		    ", guitar_brand  TEXT NOT NULL"
		    ", guitar_model  TEXT NOT NULL"
		    ", guitar_scale_length  DOUBLE"
		    ")";

		// Queries without parameter nor result bindings can be executed without a statement instantiation
		/*
		statement st{ connection, query };
		st.execute();
		*/
		connection.execute(query);
	}

	{
		prepared_statement st(connection,
		                      "INSERT INTO guitar ("
		                      "  guitar_brand, guitar_model, guitar_scale_length"
		                      ") VALUES ("
		                      "  :brand, :model, :scale_length"
		                      ") RETURNING guitar_id");

		int guitar_id{};
		st.bind_result(guitar_id);

		{
			st.bind("brand", "Gretsch");
			st.bind("model", "6120TM");
			st.bind("scale_length", 24.6);
			st.execute();
			auto fetched = st.fetch();
			assert(fetched);
			std::cout << "inserted guitar_id " << guitar_id << "\n";
			assert(guitar_id == 1);
		}

		{
			st.bind("brand", "Fender");
			st.bind("model", "Stratocaster");
			st.bind("scale_length", std::nullopt);
			st.execute();
			auto fetched = st.fetch();
			assert(fetched);
			std::cout << "inserted guitar_id " << guitar_id << "\n";
			assert(guitar_id == 2);
		}

		{
			//st.bind("brand", "Fender"); // no change!
			st.bind("model", "Telecaster");
			st.bind("scale_length", 25.5);
			st.execute();
			auto fetched = st.fetch();
			assert(fetched);
			std::cout << "inserted guitar_id " << guitar_id << "\n";
			assert(guitar_id == 3);
		}

		{
			std::string           brand;
			std::string           model;
			std::optional<double> scaleLength;

			st.bind_ref("brand", brand);
			st.bind_ref("model", model);
			st.bind_ref("scale_length", scaleLength);

			{
				brand       = "Gibson";
				model       = "Les Paul";
				scaleLength = 24.75;

				st.execute();
				auto fetched = st.fetch();
				assert(fetched);
				std::cout << "inserted guitar_id " << guitar_id << "\n";
				assert(guitar_id == 4);
			}

			{
				//brand = "Gibson"; // no change!
				model = "SG";
				//scaleLength=24.75; // no change!

				st.execute();
				auto fetched = st.fetch();
				assert(fetched);
				std::cout << "inserted guitar_id " << guitar_id << "\n";
				assert(guitar_id == 5);
			}
		}
	}
}

void test_result_by_name(connection& connection)
{
	statement st{
		connection,
		"SELECT"
		"  42  AS a"
		",'42' AS b"
		", 24  AS c"
		"" //
	};

	int         a{}, c{};
	std::string b{};

	st.bind_result("b", b);
	st.bind_result("c", c);
	st.bind_result("a", a);

	st.execute();

	auto fetched = st.fetch();
	assert(fetched);

	assert(a == 42);
	assert(b == "42");
	assert(c == 24);
}

struct MyStruct
{
	int                        a{}, c{};
	std::optional<std::string> b{};

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
	template<class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& BOOST_SERIALIZATION_NVP(a)   //
		    & BOOST_SERIALIZATION_NVP(b) //
		    & BOOST_SERIALIZATION_NVP(c) //
		    ;
	}
#else
	// When defined, this method takes precedence over serialize.
	template<class Binder>
	void bind(Binder& binder)
	{
		binder.bind("b", b);
		binder.bind("c", c);
		binder.bind("a", a);
	}
#endif
};

void test_bind_struct(connection& connection)
{
	MyStruct s{};

	{
		statement st{
			connection,
			"SELECT"
			"  42  AS a"
			",'42' AS b"
			", 24  AS c"
			"" //
		};

		st.bind_results(s);

		st.execute();

		auto fetched = st.fetch();
		assert(fetched);

		assert(s.a == 42);
		assert(s.b == "42");
		assert(s.c == 24);
	}

	{
		statement st{
			connection,
			"SELECT"
			"  :a AS a"
			", :b AS b"
			", :c AS c"
			"" //
		};

		int                        a{}, c{};
		std::optional<std::string> b{};

		st.bind(s);

		st.bind_result("b", b);
		st.bind_result("c", c);
		st.bind_result("a", a);

		st.execute();

		auto fetched = st.fetch();
		assert(fetched);

		assert(a == 42);
		assert(b == "42");
		assert(c == 24);
	}

	{
		connection.execute(R"~(
				CREATE TABLE mystruct (
				  a INTEGER NOT NULL
				, b TEXT
				, c INTEGER NOT NULL
				)
			)~");

		prepared_statement st(connection, "INSERT INTO mystruct (a, b, c) VALUES (:a, :b, :c)");

		st.bind_ref(s);

		st.execute();

		s.a = 23;
		s.b = std::nullopt;
		s.c = 99;

		st.execute();
	}
}

void playground()
{
	{
		std::variant<const int*, int> v;

		{
			int x = 42;
			v     = x;
			assert(std::holds_alternative<int>(v));
		}

		{
			int x = 24;
			v     = &x;
			assert(std::holds_alternative<const int*>(v));
		}
	}

	{
		std::optional<MyCharEnum> e   = MyCharEnum::SECOND;
		auto                      pe  = &e;
		auto                      pce = reinterpret_cast<std::optional<char>*>(pe);
		assert(pce->has_value());
		assert(pce->value() = static_cast<char>(MyCharEnum::SECOND));
		pce->reset();
		assert(!e.has_value());
		*pce = static_cast<char>(MyCharEnum::FIRST);
		assert(e.has_value());
		assert(e.value() == MyCharEnum::FIRST);
	}

	{
		std::optional<MyIntEnum> e   = MyIntEnum::SECOND;
		auto                     pe  = &e;
		auto                     pce = reinterpret_cast<std::optional<int>*>(pe);
		assert(pce->has_value());
		assert(pce->value() = static_cast<int>(MyIntEnum::SECOND));
		pce->reset();
		assert(!e.has_value());
		*pce = static_cast<int>(MyIntEnum::FIRST);
		assert(e.has_value());
		assert(e.value() == MyIntEnum::FIRST);
	}
}

void test()
{
	auto               path = std::filesystem::temp_directory_path() / "demo_sqlite3.db";
	sqlite::connection connection{ path.string() };
	std::cout << "opened database " << path << "\n";

	//	sqlite::connection connection{ ":memory:" };

	test_binding(connection);
	test_field_info(connection);
	test_table_ops(connection);
	test_result_by_name(connection);
	test_bind_struct(connection);
	playground();
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
