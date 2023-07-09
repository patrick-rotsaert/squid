//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/sqlite3/error.h>
#include <squid/sqlite3/detail/queryresults.h>
#include <squid/sqlite3/detail/sqliteapimock.h>
#include <squid/detail/conversions.h>
#include <sstream>
#include <sqlite3.h>

namespace squid {
namespace sqlite {

class QueryResultsTests : public testing::Test
{
public:
	std::vector<result>           results;       /// bound row results, by sequence
	std::map<std::string, result> named_results; /// bound row results, by name

	sqlite3*      connection = sqlite_api_mock::test_connection_shared.get();
	sqlite3_stmt* statement  = sqlite_api_mock::test_statement_shared.get();

	static std::vector<result> make_results_vector(std::size_t n)
	{
		auto v = std::vector<result>{};
		while (n--)
		{
			static int dummy;
			v.emplace_back(dummy);
		}
		return v;
	}

	static std::map<std::string, result> make_results_map(std::size_t n)
	{
		auto m = std::map<std::string, result>{};
		while (n--)
		{
			std::ostringstream os;
			os << "col" << n;
			static int dummy;
			m.insert_or_assign(os.str(), result{ dummy });
		}
		return m;
	}

	static query_results make_query_results(isqlite_api& api, const std::vector<result>& results)
	{
		return query_results{ api, sqlite_api_mock::test_connection_shared, sqlite_api_mock::test_statement_shared, results };
	}

	static query_results make_query_results(isqlite_api& api, const std::map<std::string, result>& results)
	{
		return query_results{ api, sqlite_api_mock::test_connection_shared, sqlite_api_mock::test_statement_shared, results };
	}

	template<typename T>
	QueryResultsTests& bind_result(T& ref)
	{
		this->results.emplace_back(ref);
		return *this;
	}

	template<typename T>
	QueryResultsTests& bind_result(std::string_view name, T& ref)
	{
		this->named_results.insert_or_assign(std::string{ name }, result{ ref });
		return *this;
	}
};

TEST_F(QueryResultsTests, TestFieldCount)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(42));

	EXPECT_EQ((this->make_query_results(api, std::vector<result>{}).field_count()), 42);
}

TEST_F(QueryResultsTests, TestFieldName)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(3));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(1))).WillOnce(testing::Return("second"));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(2))).WillOnce(testing::Return("third"));
	EXPECT_CALL(api, column_name(this->statement, testing::Ge(3))).Times(0);

	auto qr = this->make_query_results(api, std::vector<result>{});
	EXPECT_EQ(qr.field_name(2), "third");
	EXPECT_EQ(qr.field_name(0), "first");
	EXPECT_EQ(qr.field_name(1), "second");
}

TEST_F(QueryResultsTests, TestCtorVector)
{
	auto api = sqlite_api_mock_nice{};

	const auto n = 3;

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(n));
	{
		auto seq = testing::Sequence{};

		EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("x"));
		EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).Times(1);

		EXPECT_CALL(api, column_name(this->statement, testing::Eq(1))).WillOnce(testing::Return("x"));
		EXPECT_CALL(api, column_type(this->statement, testing::Eq(1))).Times(1);

		EXPECT_CALL(api, column_name(this->statement, testing::Eq(2))).WillOnce(testing::Return("x"));
		EXPECT_CALL(api, column_type(this->statement, testing::Eq(2))).Times(1);
	}

	this->make_query_results(api, this->make_results_vector(n));
}

TEST_F(QueryResultsTests, TestCtorMap)
{
	auto api = sqlite_api_mock_nice{};

	const auto n = 3;

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(n));
	{
		auto seq = testing::Sequence{};

		EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
		EXPECT_CALL(api, column_name(this->statement, testing::Eq(1))).WillOnce(testing::Return("second"));
		EXPECT_CALL(api, column_name(this->statement, testing::Eq(2))).WillOnce(testing::Return("third"));

		EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).Times(1);
		EXPECT_CALL(api, column_type(this->statement, testing::Eq(1))).Times(1);
		EXPECT_CALL(api, column_type(this->statement, testing::Eq(2))).Times(1);
	}

	auto dummy = int{};
	this->bind_result("first", dummy).bind_result("second", dummy).bind_result("third", dummy);

	this->make_query_results(api, this->named_results);
}

TEST_F(QueryResultsTests, TestCtorVectorTooBig)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(3));

	EXPECT_ANY_THROW((this->make_query_results(api, this->make_results_vector(4))));
}

TEST_F(QueryResultsTests, TestCtorVectorColumnNameError)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(2));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(1))).WillOnce(testing::ReturnNull());

	EXPECT_ANY_THROW((this->make_query_results(api, this->make_results_vector(2))));
}

TEST_F(QueryResultsTests, TestCtorMapTooBig)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(3));

	EXPECT_ANY_THROW((this->make_query_results(api, this->make_results_map(4))));
}

TEST_F(QueryResultsTests, TestCtorMapColumnNameError)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(2));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(1))).WillOnce(testing::ReturnNull());

	EXPECT_ANY_THROW((this->make_query_results(api, this->make_results_map(2))));
}

TEST_F(QueryResultsTests, TestCtorMapColumnNameNotFound)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));

	auto dummy = int{};
	this->bind_result("does_not_exist", dummy);

	EXPECT_ANY_THROW((this->make_query_results(api, this->named_results)));
}

TEST_F(QueryResultsTests, TestFetchNullInNonNullable)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NULL));

	auto res = int{};
	this->bind_result(res);

	auto qr = this->make_query_results(api, this->results);
	EXPECT_ANY_THROW(qr.fetch());
}

TEST_F(QueryResultsTests, TestFetchNullInNullable)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NULL));

	auto res = std::optional<int>{};
	res      = 42;
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_FALSE(res.has_value());
}

static constexpr auto SQLITE_NOT_NULL = SQLITE_NULL + 1; // value does not matter, as long as it is different from SQLITE_NULL

TEST_F(QueryResultsTests, TestFetchNonNullInNullable)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	auto res = std::optional<std::int16_t>{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_TRUE(res.has_value());
	EXPECT_EQ(res.value(), 42);
}

TEST_F(QueryResultsTests, TestFetchBool)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int(this->statement, testing::Eq(0))).WillOnce(testing::Return(1));

	auto res = false;
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_TRUE(res);
}

TEST_F(QueryResultsTests, TestFetchChar)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0))).WillOnce(testing::Return(reinterpret_cast<const unsigned char*>("#")));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(1));

	auto res = char{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, '#');
}

TEST_F(QueryResultsTests, TestFetchCharLengthNotOne)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0))).WillOnce(testing::Return(reinterpret_cast<const unsigned char*>("##")));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(2));

	auto res = char{};
	this->bind_result(res);

	auto qr = this->make_query_results(api, this->results);
	EXPECT_ANY_THROW(qr.fetch());
}

TEST_F(QueryResultsTests, TestFetchSignedChar)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	signed char res{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchUnsignedSignedChar)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	unsigned char res{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchInt16)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	auto res = std::int16_t{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchUInt16)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	auto res = std::uint16_t{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchInt32)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	auto res = std::int32_t{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchUInt32)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int64(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	auto res = std::uint32_t{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchInt64)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int64(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	auto res = std::int64_t{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchUInt64)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_int64(this->statement, testing::Eq(0))).WillOnce(testing::Return(42));

	auto res = std::uint64_t{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, 42);
}

TEST_F(QueryResultsTests, TestFetchFloat)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_double(this->statement, testing::Eq(0))).WillOnce(testing::Return(42.0));

	auto res = float{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_FLOAT_EQ(res, 42.0f);
}

TEST_F(QueryResultsTests, TestFetchDouble)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_double(this->statement, testing::Eq(0))).WillOnce(testing::Return(42.0));

	auto res = double{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_DOUBLE_EQ(res, 42.0);
}

TEST_F(QueryResultsTests, TestFetchLongDouble)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_double(this->statement, testing::Eq(0))).WillOnce(testing::Return(42.0));

	long double res{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_DOUBLE_EQ(res, 42.0);
}

TEST_F(QueryResultsTests, TestFetchString)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>("hello")));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(5));

	auto res = std::string{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, "hello");
}

TEST_F(QueryResultsTests, TestFetchStringFailGetText)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0))).WillOnce(testing::ReturnNull());

	auto res = std::string{};
	this->bind_result(res);

	auto qr = this->make_query_results(api, this->results);
	EXPECT_ANY_THROW(qr.fetch());
}

TEST_F(QueryResultsTests, TestFetchStringFailGetBytes)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>("hello")));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(-1));

	auto res = std::string{};
	this->bind_result(res);

	auto qr = this->make_query_results(api, this->results);
	EXPECT_ANY_THROW(qr.fetch());
}

static const byte_string test_byte_string{ reinterpret_cast<const unsigned char*>("hello"), 5 };

TEST_F(QueryResultsTests, TestFetchByteString)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_blob(this->statement, testing::Eq(0))).WillOnce(testing::Return(test_byte_string.c_str()));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(test_byte_string.length()));

	auto res = byte_string{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, test_byte_string);
}

TEST_F(QueryResultsTests, TestFetchEmptyByteString)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_blob(this->statement, testing::Eq(0))).WillOnce(testing::ReturnNull());
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(0));

	auto res = test_byte_string;
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_TRUE(res.empty());
}

TEST_F(QueryResultsTests, TestFetchByteStringFailGetBlob)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_blob(this->statement, testing::Eq(0))).WillOnce(testing::ReturnNull());
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(test_byte_string.length()));

	auto res = byte_string{};
	this->bind_result(res);

	auto qr = this->make_query_results(api, this->results);
	EXPECT_ANY_THROW(qr.fetch());
}

TEST_F(QueryResultsTests, TestFetchByteStringFailGetBytes)
{
	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_blob(this->statement, testing::Eq(0))).WillOnce(testing::Return(test_byte_string.c_str()));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(-1));

	auto res = byte_string{};
	this->bind_result(res);

	auto qr = this->make_query_results(api, this->results);
	EXPECT_ANY_THROW(qr.fetch());
}

TEST_F(QueryResultsTests, TestFetchTimepoint)
{
	auto api = sqlite_api_mock_nice{};

	const auto tp = std::chrono::sys_days{ std::chrono::year{ 2023 } / std::chrono::month{ 7 } / 28 } + std::chrono::hours{ 21 } +
	                std::chrono::minutes{ 25 } + std::chrono::seconds{ 2 };
	const auto s = time_point_to_string(tp);

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>(s.c_str())));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(s.length()));

	auto res = time_point{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, tp);
}

TEST_F(QueryResultsTests, TestFetchDate)
{
	auto api = sqlite_api_mock_nice{};

	const auto dt = std::chrono::sys_days{ std::chrono::year{ 2023 } / std::chrono::month{ 7 } / 28 };
	const auto s  = date_to_string(dt);

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>(s.c_str())));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(s.length()));

	auto res = date{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, dt);
}

TEST_F(QueryResultsTests, TestFetchTimeOfDay)
{
	auto api = sqlite_api_mock_nice{};

	const auto tod = time_of_day{ std::chrono::hours{ 21 } + std::chrono::minutes{ 25 } + std::chrono::seconds{ 2 } };
	const auto s   = time_of_day_to_string(tod);

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>(s.c_str())));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(s.length()));

	auto res = time_of_day{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res.to_duration(), tod.to_duration());
}

#ifdef SQUID_HAVE_BOOST_DATE_TIME
TEST_F(QueryResultsTests, TestFetchBoostPtime)
{
	auto api = sqlite_api_mock_nice{};

	const auto pt = boost::posix_time::ptime{ boost::gregorian::date{ 2023, 7, 28 }, boost::posix_time::time_duration{ 21, 25, 2 } };
	const auto s  = boost_ptime_to_string(pt);

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>(s.c_str())));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(s.length()));

	auto res = boost::posix_time::ptime{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, pt);
}

TEST_F(QueryResultsTests, TestFetchBoostDate)
{
	auto api = sqlite_api_mock_nice{};

	const auto dt = boost::gregorian::date{ 2023, 7, 28 };
	const auto s  = boost_date_to_string(dt);

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>(s.c_str())));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(s.length()));

	auto res = boost::gregorian::date{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, dt);
}

TEST_F(QueryResultsTests, TestFetchBoostTimeDuration)
{
	auto api = sqlite_api_mock_nice{};

	const auto td = boost::posix_time::time_duration{ 21, 25, 2 };
	const auto s  = boost_time_duration_to_string(td);

	EXPECT_CALL(api, column_count(this->statement)).WillOnce(testing::Return(1));
	EXPECT_CALL(api, column_name(this->statement, testing::Eq(0))).WillOnce(testing::Return("first"));
	EXPECT_CALL(api, column_type(this->statement, testing::Eq(0))).WillOnce(testing::Return(SQLITE_NOT_NULL));
	EXPECT_CALL(api, column_text(this->statement, testing::Eq(0)))
	    .WillOnce(testing::Return(reinterpret_cast<const unsigned char*>(s.c_str())));
	EXPECT_CALL(api, column_bytes(this->statement, testing::Eq(0))).WillOnce(testing::Return(s.length()));

	auto res = boost::posix_time::time_duration{};
	this->bind_result(res);

	this->make_query_results(api, this->results).fetch();
	EXPECT_EQ(res, td);
}
#endif

} // namespace sqlite
} // namespace squid
