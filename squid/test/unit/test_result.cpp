#include <gtest/gtest.h>
#include <squid/result.h>

namespace squid {

TEST(ResultTest, Construction)
{
#define TEST_CONSTRUCTION(TYPE)                                                                                                            \
	do                                                                                                                                     \
	{                                                                                                                                      \
		{                                                                                                                                  \
			TYPE   src{};                                                                                                                  \
			result x{ src };                                                                                                               \
			EXPECT_TRUE(std::holds_alternative<result::non_nullable_type>(x.value()));                                                     \
			const auto& value = std::get<result::non_nullable_type>(x.value());                                                            \
			EXPECT_TRUE(std::holds_alternative<TYPE*>(value));                                                                             \
			const auto& evalue = std::get<TYPE*>(value);                                                                                   \
			EXPECT_EQ(evalue, &src);                                                                                                       \
		}                                                                                                                                  \
		{                                                                                                                                  \
			std::optional<TYPE> src{};                                                                                                     \
			result              x{ src };                                                                                                  \
			EXPECT_TRUE(std::holds_alternative<result::nullable_type>(x.value()));                                                         \
			const auto& value = std::get<result::nullable_type>(x.value());                                                                \
			EXPECT_TRUE(std::holds_alternative<std::optional<TYPE>*>(value));                                                              \
			const auto& evalue = std::get<std::optional<TYPE>*>(value);                                                                    \
			EXPECT_EQ(evalue, &src);                                                                                                       \
		}                                                                                                                                  \
	} while (false)

	TEST_CONSTRUCTION(bool);
	TEST_CONSTRUCTION(char);
	TEST_CONSTRUCTION(signed char);
	TEST_CONSTRUCTION(unsigned char);
	TEST_CONSTRUCTION(std::int16_t);
	TEST_CONSTRUCTION(std::uint16_t);
	TEST_CONSTRUCTION(std::int32_t);
	TEST_CONSTRUCTION(std::uint32_t);
	TEST_CONSTRUCTION(std::int64_t);
	TEST_CONSTRUCTION(std::uint64_t);
	TEST_CONSTRUCTION(float);
	TEST_CONSTRUCTION(double);
	TEST_CONSTRUCTION(long double);
	TEST_CONSTRUCTION(std::string);
	TEST_CONSTRUCTION(byte_string);
	TEST_CONSTRUCTION(time_point);
	TEST_CONSTRUCTION(date);
	TEST_CONSTRUCTION(time_of_day);
}

TEST(ResultTest, ConstructionFromEnum)
{
#define TEST_CONSTRUCTION_FROM_ENUM(TYPE)                                                                                                  \
	do                                                                                                                                     \
	{                                                                                                                                      \
		using base = std::underlying_type_t<TYPE>;                                                                                         \
		{                                                                                                                                  \
			TYPE   src{};                                                                                                                  \
			result x{ src };                                                                                                               \
			EXPECT_TRUE(std::holds_alternative<result::non_nullable_type>(x.value()));                                                     \
			const auto& value = std::get<result::non_nullable_type>(x.value());                                                            \
			EXPECT_TRUE(std::holds_alternative<base*>(value));                                                                             \
			const auto& evalue = std::get<base*>(value);                                                                                   \
			EXPECT_EQ((void*)(evalue), (void*)(&src));                                                                                     \
		}                                                                                                                                  \
		{                                                                                                                                  \
			std::optional<TYPE> src{};                                                                                                     \
			result              x{ src };                                                                                                  \
			EXPECT_TRUE(std::holds_alternative<result::nullable_type>(x.value()));                                                         \
			const auto& value = std::get<result::nullable_type>(x.value());                                                                \
			EXPECT_TRUE(std::holds_alternative<std::optional<base>*>(value));                                                              \
			const auto& evalue = std::get<std::optional<base>*>(value);                                                                    \
			EXPECT_EQ((void*)(evalue), (void*)(&src));                                                                                     \
		}                                                                                                                                  \
	} while (false)

	enum MyEnum
	{
		MyEnum_FIRST  = 42,
		MyEnum_SECOND = 43,
		MyEnum_THIRD  = 44,
	};
	TEST_CONSTRUCTION_FROM_ENUM(MyEnum);

	enum class MyCharEnum : char
	{
		FIRST  = 'a',
		SECOND = 'b',
		THIRD  = 'c',
	};
	TEST_CONSTRUCTION_FROM_ENUM(MyCharEnum);

	enum class MyIntEnum : int
	{
		FIRST  = 42,
		SECOND = 43,
		THIRD  = 44,
	};
	TEST_CONSTRUCTION_FROM_ENUM(MyIntEnum);

	enum class MyInt64Enum : std::int64_t
	{
		FIRST  = 42,
		SECOND = 43,
		THIRD  = 44,
	};
	TEST_CONSTRUCTION_FROM_ENUM(MyInt64Enum);
}

} // namespace squid
