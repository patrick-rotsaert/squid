#include <gtest/gtest.h>
#include <squid/postgresql/detail/conversions.h>

namespace squid {
namespace postgresql {

namespace {

constexpr char text_data[] =
    "\\x"
    "1a"
    "2b"
    "3c"
    "4d"
    "5e"
    "6f"
    "78"
    "90"
    "a1"
    "b2"
    "c3"
    "d4"
    "e5"
    "f6"
    "78";

constexpr unsigned char binary_data[] = {
	0x1a, //
	0x2b, //
	0x3c, //
	0x4d, //
	0x5e, //
	0x6f, //
	0x78, //
	0x90, //
	0xa1, //
	0xb2, //
	0xc3, //
	0xd4, //
	0xe5, //
	0xf6, //
	0x78  //
};

} // namespace

TEST(PostgresqlConversionsTest, HexStringToBinary)
{
	const byte_string_view binary{ binary_data, sizeof(binary_data) };
	EXPECT_EQ(hex_string_to_binary(text_data), binary);
	EXPECT_EQ(hex_string_to_binary("\\x"), byte_string_view{});
}

TEST(PostgresqlConversionsTest, HexStringToBinaryErrors)
{
	EXPECT_THROW(hex_string_to_binary("ff"), std::runtime_error);
	EXPECT_THROW(hex_string_to_binary("\\Xff"), std::runtime_error);
	EXPECT_THROW(hex_string_to_binary("\\xfff"), std::runtime_error);
}

TEST(PostgresqlConversionsTest, BinaryToHexString)
{
	const byte_string_view binary{ binary_data, sizeof(binary_data) };
	EXPECT_STRCASEEQ(binary_to_hex_string(binary).c_str(), text_data);
}

} // namespace postgresql
} // namespace squid
