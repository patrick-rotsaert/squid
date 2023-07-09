//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <gtest/gtest.h>
#include <squid/sqlite3/detail/queryparameters.h>
#include <squid/sqlite3/detail/sqliteapimock.h>
#include <sqlite3.h>

namespace squid {
namespace sqlite {

class QueryParameterTests : public testing::Test
{
public:
	std::map<std::string, parameter> parameters; /// bound query parameters

	template<typename... Args>
	void upsert_parameter(std::string_view name, Args&&... args)
	{
		this->parameters.insert_or_assign(std::string{ name }, parameter{ std::forward<Args>(args)... });
	}

	template<typename T>
	void bind(std::string_view name, const T& value)
	{
		this->upsert_parameter(name, value, parameter::by_value{});
	}
};

TEST_F(QueryParameterTests, TestBindParameterIndexWithColon)
{
	constexpr int param_value = 42;
	constexpr int param_index = 7;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));
	EXPECT_CALL(api, bind_int(sqlite_api_mock::test_statement, param_index, param_value)).WillOnce(testing::Return(SQLITE_OK));

	query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters);
}

TEST_F(QueryParameterTests, TestBindParameterIndexWithMonkeyTail)
{
	constexpr int param_value = 42;
	constexpr int param_index = 7;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(-1));
	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq("@name"))).WillOnce(testing::Return(param_index));
	EXPECT_CALL(api, bind_int(sqlite_api_mock::test_statement, param_index, param_value)).WillOnce(testing::Return(SQLITE_OK));

	query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters);
}

TEST_F(QueryParameterTests, TestBindParameterIndexWithDollar)
{
	constexpr int param_value = 42;
	constexpr int param_index = 7;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(-1));
	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq("@name"))).WillOnce(testing::Return(-1));
	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq("$name"))).WillOnce(testing::Return(param_index));
	EXPECT_CALL(api, bind_int(sqlite_api_mock::test_statement, param_index, param_value)).WillOnce(testing::Return(SQLITE_OK));

	query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters);
}

TEST_F(QueryParameterTests, TestBindParameterIndexNotFound)
{
	constexpr int param_value = 42;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(-1));
	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq("@name"))).WillOnce(testing::Return(-1));
	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq("$name"))).WillOnce(testing::Return(-1));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindNullError)
{
	constexpr int param_index = 7;

	this->bind("name", std::nullopt);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api, bind_null(sqlite_api_mock::test_statement, param_index)).WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindBoolError)
{
	constexpr int  param_index = 7;
	constexpr bool param_value = true;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api, bind_int(sqlite_api_mock::test_statement, param_index, 1)).WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindCharError)
{
	constexpr int  param_index = 7;
	constexpr char param_value = '*';

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api, bind_text(sqlite_api_mock::test_statement, param_index, testing::StrEq("*"), 1, SQLITE_STATIC))
	    .WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindIntError)
{
	constexpr int param_index = 7;
	constexpr int param_value = 42;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api, bind_int(sqlite_api_mock::test_statement, param_index, param_value)).WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindInt64Error)
{
	constexpr int     param_index = 7;
	constexpr int64_t param_value = 42;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api, bind_int64(sqlite_api_mock::test_statement, param_index, param_value)).WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindDoubleError)
{
	constexpr int    param_index = 7;
	constexpr double param_value = 42;

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api, bind_double(sqlite_api_mock::test_statement, param_index, param_value)).WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindStringError)
{
	constexpr int param_index = 7;
	const auto    param_value = std::string{ "fubar" };

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api,
	            bind_text(sqlite_api_mock::test_statement, param_index, testing::StrEq(param_value), param_value.length(), SQLITE_STATIC))
	    .WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

TEST_F(QueryParameterTests, TestBindByteStringError)
{
	constexpr int param_index = 7;
	unsigned char data[4]     = {};
	const auto    param_value = byte_string{ data, sizeof(data) };

	this->bind("name", param_value);

	auto api = sqlite_api_mock_nice{};

	EXPECT_CALL(api, bind_parameter_index(sqlite_api_mock::test_statement, testing::StrEq(":name"))).WillOnce(testing::Return(param_index));

	EXPECT_CALL(api, bind_blob(sqlite_api_mock::test_statement, param_index, testing::_, param_value.length(), SQLITE_STATIC))
	    .WillOnce(testing::Return(SQLITE_ERROR));

	EXPECT_ANY_THROW((query_parameters::bind(api, *sqlite_api_mock::test_connection, *sqlite_api_mock::test_statement, this->parameters)));
}

} // namespace sqlite
} // namespace squid
