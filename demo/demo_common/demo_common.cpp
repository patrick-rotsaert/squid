//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "demo_common.h"

#include "squid/statement.h"
#include "squid/preparedstatement.h"
#include "squid/transaction.h"
#include "squid/config.h"

#include "squid/detail/conversions.h"

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
#include <boost/serialization/nvp.hpp>
#endif
#ifdef SQUID_HAVE_BOOST_DATE_TIME
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#endif

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>
#include <optional>
#include <memory>
#include <cassert>

namespace squid {
namespace demo {

namespace {

std::string to_hex_string(byte_string_view bytes)
{
	std::ostringstream out;
	for (const auto byte : bytes)
	{
		out << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint16_t>(byte);
	}
	return out.str();
}

} // namespace

void demo_bindings(connection& connection)
{
	std::optional<double> optdouble = std::nullopt;
	unsigned char         bytes[]   = { 0xDE, 0xAD, 0xBE, 0xEF };
	byte_string           bytes2{ bytes, sizeof(bytes) };
	std::string           s{ "foo" };

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
#ifdef SQUID_HAVE_BOOST_DATE_TIME
		", :s as s"
		", :t as t"
		", :u as u"
#endif
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
#ifdef SQUID_HAVE_BOOST_DATE_TIME
	st.bind("s", boost::posix_time::microsec_clock::universal_time());
	st.bind("t", boost::gregorian::day_clock::universal_day());
	st.bind("u", boost::posix_time::time_duration{ 23, 59, 45 });
#endif

	char                                  a = -1;
	float                                 b = -1;
	std::string                           c{}, d{}, f{};
	std::optional<double>                 e{}, g{}, h{};
	time_point                            tp{}, tp2{};
	time_of_day                           tm{};
	MyCharEnum                            i{};
	MyIntEnum                             j{};
	MyEnum                                jj{};
	bool                                  k{};
	byte_string                           l{}, m{}, n{};
	std::chrono::system_clock::time_point o{};
	std::string                           p{}, q{};
	date                                  r{};
#ifdef SQUID_HAVE_BOOST_DATE_TIME
	boost::posix_time::ptime         b_s{};
	boost::gregorian::date           b_t{};
	boost::posix_time::time_duration b_u{};
#endif

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
	st.bind_results(a, b, c, d, e, tp, tp2, tm, f, g, h, i, j, jj, k, l, m, n, o, p, q, r);

#ifdef SQUID_HAVE_BOOST_DATE_TIME
	st.bind_results(b_s, b_t, b_u);
#endif

	st.execute();

	auto fetched = st.fetch();
	assert(fetched);

	std::cout << "a=" << a << ", b=" << b << ", c=" << std::quoted(c) << ", d=" << std::quoted(d)
	          << ", e=" << (e ? std::to_string(e.value()) : "<NULL>") << ", m=" << to_hex_string(m) << ", tp=" << time_point_to_string(tp)
	          << "Z"
	          << ", tp2=" << time_point_to_string(tp2) << "Z"
#ifdef SQUID_HAVE_BOOST_DATE_TIME
	          << ", s=" << b_s << ", t=" << b_t << ", u=" << b_u
#endif
	          << "\n";

	assert(i == MyCharEnum::SECOND);
	assert(j == MyIntEnum::SECOND);
	assert(jj == MyEnum_SECOND);

	fetched = st.fetch();
	assert(!fetched);
}

void demo_field_info(connection& connection)
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

void demo_result_by_name(connection& connection)
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

void demo_query_stream(connection& connection)
{
	statement st{ connection };

	int bar{};
	(st << "SELECT :foo AS bar").bind_result("bar", bar).bind("foo", 42);

	int baz{};
	(st << ", " << 24 << " AS baz").bind_result("baz", baz);

	st.execute();
	auto fetched = st.fetch();

	assert(fetched);
	assert(bar == 42);
	assert(baz == 24);
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

void demo_bind_struct(connection& connection)
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
			DROP TABLE IF EXISTS mystruct
		)~");

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

class guitar_inserter
{
	std::unique_ptr<prepared_statement> st_;
	std::unique_ptr<prepared_statement> st_fetch_;

public:
	explicit guitar_inserter(connection& connection, Backend backend, int& guitar_id)
	    : st_{}
	    , st_fetch_{}
	{
		switch (backend)
		{
		case Backend::SQLITE3:
		case Backend::POSTGRESQL:
			this->st_ = std::make_unique<prepared_statement>(connection, R"~(
				INSERT INTO guitar (
				  guitar_brand, guitar_model, guitar_scale_length
				) VALUES (
				  :brand, :model, :scale_length
				) RETURNING guitar_id
			)~");
			this->st_->bind_result(guitar_id);
			break;
		case Backend::MYSQL:
			this->st_       = std::make_unique<prepared_statement>(connection, R"~(
				INSERT INTO guitar (
				  guitar_brand, guitar_model, guitar_scale_length
				) VALUES (
				  :brand, :model, :scale_length
				)
			)~");
			this->st_fetch_ = std::make_unique<prepared_statement>(connection, R"~(
				SELECT LAST_INSERT_ID()
			)~");
			this->st_fetch_->bind_result(guitar_id);
			break;
		}
	}

	prepared_statement& statement()
	{
		return *this->st_;
	}

	void execute()
	{
		this->st_->execute();
		//std::cout << "affected_rows:" << this->st_->affected_rows() << '\n';

		if (this->st_fetch_)
		{
			this->st_fetch_->execute();
			auto fetched = this->st_fetch_->fetch();
			assert(fetched);
		}
		else
		{
			auto fetched = this->st_->fetch();
			assert(fetched);
		}
	}
};

void demo_table_ops(connection& connection, Backend backend)
{
	transaction tr{ connection };

	connection.execute(R"~(
		DROP TABLE IF EXISTS guitar
	)~");

	switch (backend)
	{
	case Backend::SQLITE3:
		connection.execute(R"~(
			CREATE TABLE guitar (
			  guitar_id  INTEGER PRIMARY KEY
			, guitar_brand  TEXT NOT NULL
			, guitar_model  TEXT NOT NULL
			, guitar_scale_length  DOUBLE
			)
		)~");
		break;
	case Backend::POSTGRESQL:
		connection.execute(R"~(
			CREATE TABLE guitar (
			  guitar_id  SERIAL PRIMARY KEY
			, guitar_brand  TEXT NOT NULL
			, guitar_model  TEXT NOT NULL
			, guitar_scale_length  DOUBLE PRECISION
			)
		)~");
		break;
	case Backend::MYSQL:
		connection.execute(R"~(
			CREATE TABLE guitar (
			  guitar_id  INTEGER AUTO_INCREMENT PRIMARY KEY
			, guitar_brand  TEXT NOT NULL
			, guitar_model  TEXT NOT NULL
			, guitar_scale_length  DOUBLE PRECISION
			)
		)~");
		break;
	}

	{
		int guitar_id{};

		guitar_inserter inserter{ connection, backend, guitar_id };

		auto& st = inserter.statement();

		{
			st.bind("brand", "Gretsch");
			st.bind("model", "6120TM");
			st.bind("scale_length", 24.6);
			inserter.execute();
			std::cout << "inserted guitar_id " << guitar_id << "\n";
			assert(guitar_id == 1);
		}

		{
			st.bind("brand", "Fender");
			st.bind("model", "Stratocaster");
			st.bind("scale_length", std::nullopt);
			inserter.execute();
			std::cout << "inserted guitar_id " << guitar_id << "\n";
			assert(guitar_id == 2);
		}

		{
			//st.bind("brand", "Fender"); // no change!
			st.bind("model", "Telecaster");
			st.bind("scale_length", 25.5);
			inserter.execute();
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

				inserter.execute();
				std::cout << "inserted guitar_id " << guitar_id << "\n";
				assert(guitar_id == 4);
			}

			{
				//brand = "Gibson"; // no change!
				model = "SG";
				//scaleLength=24.75; // no change!

				inserter.execute();
				std::cout << "inserted guitar_id " << guitar_id << "\n";
				assert(guitar_id == 5);
			}
		}
	}

	tr.commit();
}

void demo_all(connection& connection, Backend backend)
{
	demo_bindings(connection);
	demo_field_info(connection);
	demo_result_by_name(connection);
	demo_query_stream(connection);
	demo_bind_struct(connection);
	demo_table_ops(connection, backend);
}

} // namespace demo
} // namespace squid
