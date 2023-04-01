#include <gtest/gtest.h>
#include <squid/parameter.h>

#if defined(_MSC_VER)
#pragma warning(disable : 4244)
#endif

namespace squid {

TEST(ParameterTest, ByValueConstructFromScalars)
{
#define TEST_CONSTRUCT_FROM_SCALAR(TYPE, VALUE)                                                                                            \
	do                                                                                                                                     \
	{                                                                                                                                      \
		{                                                                                                                                  \
			TYPE      src{ VALUE };                                                                                                        \
			parameter x{ src, parameter::by_value{} };                                                                                     \
			EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));                                                         \
			const auto& value = std::get<parameter::value_type>(x.value());                                                                \
			EXPECT_TRUE(std::holds_alternative<TYPE>(value));                                                                              \
			const auto& evalue = std::get<TYPE>(value);                                                                                    \
			EXPECT_EQ(evalue, src);                                                                                                        \
		}                                                                                                                                  \
		{                                                                                                                                  \
			parameter x{ static_cast<TYPE>(VALUE), parameter::by_value{} };                                                                \
			EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));                                                         \
			const auto& value = std::get<parameter::value_type>(x.value());                                                                \
			EXPECT_TRUE(std::holds_alternative<TYPE>(value));                                                                              \
			const auto& evalue = std::get<TYPE>(value);                                                                                    \
			EXPECT_EQ(evalue, static_cast<TYPE>(VALUE));                                                                                   \
		}                                                                                                                                  \
	} while (false)

	TEST_CONSTRUCT_FROM_SCALAR(bool, true);
	TEST_CONSTRUCT_FROM_SCALAR(char, 'A');
	TEST_CONSTRUCT_FROM_SCALAR(signed char, 42);
	TEST_CONSTRUCT_FROM_SCALAR(unsigned char, 42);
	TEST_CONSTRUCT_FROM_SCALAR(std::int16_t, 42);
	TEST_CONSTRUCT_FROM_SCALAR(std::uint16_t, 42);
	TEST_CONSTRUCT_FROM_SCALAR(std::int32_t, 42);
	TEST_CONSTRUCT_FROM_SCALAR(std::uint32_t, 42);
	TEST_CONSTRUCT_FROM_SCALAR(std::int64_t, 42);
	TEST_CONSTRUCT_FROM_SCALAR(std::uint64_t, 42);
	TEST_CONSTRUCT_FROM_SCALAR(float, 42);
	TEST_CONSTRUCT_FROM_SCALAR(double, 42);
	TEST_CONSTRUCT_FROM_SCALAR(long double, 42);
}

TEST(ParameterTest, ByValueConstructFromEnum)
{
	enum Enum
	{
		FIRST,
		SECOND
	} src{};
	parameter x{ src, parameter::by_value{} };
	EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
	const auto& value = std::get<parameter::value_type>(x.value());
	EXPECT_TRUE(std::holds_alternative<std::underlying_type_t<Enum>>(value));
}

TEST(ParameterTest, ByValueConstructFromEnumClassInteger)
{
	enum class Enum : int
	{
		FIRST,
		SECOND
	} src{};
	parameter x{ src, parameter::by_value{} };
	EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
	const auto& value = std::get<parameter::value_type>(x.value());
	EXPECT_TRUE(std::holds_alternative<std::underlying_type_t<Enum>>(value));
}

TEST(ParameterTest, ByValueConstructFromEnumClassChar)
{
	enum class Enum : char
	{
		FIRST  = 'A',
		SECOND = 'B'
	} src{ Enum::SECOND };
	parameter x{ src, parameter::by_value{} };
	EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
	const auto& value = std::get<parameter::value_type>(x.value());
	EXPECT_TRUE(std::holds_alternative<char>(value));
	const auto& evalue = std::get<char>(value);
	EXPECT_EQ(evalue, 'B');
}

TEST(ParameterTest, ByValueConstructFromString)
{
	{
		std::string src{ "foo" };
		parameter   x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<std::string>(value));
		const auto& evalue = std::get<std::string>(value);
		EXPECT_EQ(evalue, src);
	}
	{
		std::string src{ "foo" };
		parameter   x{ std::string{ src }, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<std::string>(value));
		const auto& evalue = std::get<std::string>(value);
		EXPECT_EQ(evalue, src);
	}
}

TEST(ParameterTest, ByValueConstructFromStringView)
{
	{
		std::string_view src{ "foo" };
		parameter        x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<std::string_view>(value));
		const auto& evalue = std::get<std::string_view>(value);
		EXPECT_EQ(evalue, src);
		EXPECT_EQ(evalue.data(), src.data());
	}
	{
		std::string_view src{ "foo" };
		parameter        x{ std::string_view{ src }, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<std::string_view>(value));
		const auto& evalue = std::get<std::string_view>(value);
		EXPECT_EQ(evalue, src);
		EXPECT_EQ(evalue.data(), src.data());
	}
}

TEST(ParameterTest, ByValueConstructFromCharPointer)
{
	const char* src{ "foo" };
	parameter   x{ src, parameter::by_value{} };
	EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
	const auto& value = std::get<parameter::value_type>(x.value());
	EXPECT_TRUE(std::holds_alternative<std::string_view>(value));
	const auto& evalue = std::get<std::string_view>(value);
	EXPECT_EQ(evalue, src);
	EXPECT_EQ(evalue.data(), src);
	EXPECT_EQ(evalue.length(), 3);
}

TEST(ParameterTest, ByValueConstructFromByteString)
{
	{
		byte_string src{ reinterpret_cast<const unsigned char*>("foo"), 3 };
		parameter   x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<byte_string>(value));
		const auto& evalue = std::get<byte_string>(value);
		EXPECT_EQ(evalue, src);
	}
	{
		byte_string src{ reinterpret_cast<const unsigned char*>("foo"), 3 };
		parameter   x{ byte_string{ src }, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<byte_string>(value));
		const auto& evalue = std::get<byte_string>(value);
		EXPECT_EQ(evalue, src);
	}
}

TEST(ParameterTest, ByValueConstructFromByteStringView)
{
	{
		byte_string_view src{ reinterpret_cast<const unsigned char*>("foo"), 3 };
		parameter        x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<byte_string_view>(value));
		const auto& evalue = std::get<byte_string_view>(value);
		EXPECT_EQ(evalue, src);
		EXPECT_EQ(evalue.data(), src.data());
	}
	{
		byte_string_view src{ reinterpret_cast<const unsigned char*>("foo"), 3 };
		parameter        x{ byte_string_view{ src }, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<byte_string_view>(value));
		const auto& evalue = std::get<byte_string_view>(value);
		EXPECT_EQ(evalue, src);
		EXPECT_EQ(evalue.data(), src.data());
	}
}

TEST(ParameterTest, ByValueConstructFromTimePoint)
{
	{
		time_point src{ std::chrono::system_clock::now() };
		parameter  x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<time_point>(value));
		const auto& evalue = std::get<time_point>(value);
		EXPECT_EQ(evalue, src);
	}
	{
		time_point src{ std::chrono::system_clock::now() };
		parameter  x{ time_point{ src }, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<time_point>(value));
		const auto& evalue = std::get<time_point>(value);
		EXPECT_EQ(evalue, src);
	}
}

TEST(ParameterTest, ByValueConstructFromDate)
{
	{
		date      src{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) };
		parameter x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<date>(value));
		const auto& evalue = std::get<date>(value);
		EXPECT_EQ(evalue, src);
	}
	{
		date      src{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) };
		parameter x{ date{ src }, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<date>(value));
		const auto& evalue = std::get<date>(value);
		EXPECT_EQ(evalue, src);
	}
}

TEST(ParameterTest, ByValueConstructFromTimeOfDay)
{
	{
		time_of_day src{ std::chrono::hours{ 12 } };
		parameter   x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<time_of_day>(value));
		const auto& evalue = std::get<time_of_day>(value);
		EXPECT_EQ(evalue.to_duration(), src.to_duration());
	}
	{
		time_of_day src{ std::chrono::hours{ 12 } };
		parameter   x{ time_of_day{ src }, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<time_of_day>(value));
		const auto& evalue = std::get<time_of_day>(value);
		EXPECT_EQ(evalue.to_duration(), src.to_duration());
	}
}

TEST(ParameterTest, ByValueConstructFromStdOptionalWithoutValue)
{
	{
		std::optional<int> src{ std::nullopt };
		parameter          x{ src, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<std::nullopt_t>(value));
	}
	{
		parameter x{ std::nullopt, parameter::by_value{} };
		EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
		const auto& value = std::get<parameter::value_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<std::nullopt_t>(value));
	}
}

TEST(ParameterTest, ByValueConstructFromStdOptionalWithValue)
{
	std::optional<int> src{ 42 };
	parameter          x{ src, parameter::by_value{} };
	EXPECT_TRUE(std::holds_alternative<parameter::value_type>(x.value()));
	const auto& value = std::get<parameter::value_type>(x.value());
	EXPECT_TRUE(std::holds_alternative<int>(value));
	const auto& evalue = std::get<int>(value);
	EXPECT_EQ(evalue, src);
}

TEST(ParameterTest, ByReferenceConstructFromNonOptional)
{
	int       src{ 42 };
	parameter x{ src, parameter::by_reference{} };
	EXPECT_TRUE(std::holds_alternative<parameter::reference_type>(x.value()));
	const auto& ref = std::get<parameter::reference_type>(x.value());
	EXPECT_TRUE(std::holds_alternative<parameter::pointer_type>(ref));
	const auto& ptr = std::get<parameter::pointer_type>(ref);
	EXPECT_TRUE(std::holds_alternative<const int*>(ptr));
	const auto& eptr = std::get<const int*>(ptr);
	EXPECT_EQ(eptr, &src);
}

TEST(ParameterTest, ByReferenceConstructFromEnum)
{
	{
		enum Enum
		{
			FIRST,
			SECOND
		} src{};
		parameter x{ src, parameter::by_reference{} };
		EXPECT_TRUE(std::holds_alternative<parameter::reference_type>(x.value()));
		const auto& ref = std::get<parameter::reference_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<parameter::pointer_type>(ref));
		const auto& ptr            = std::get<parameter::pointer_type>(ref);
		using enum_underlying_type = std::underlying_type_t<Enum>;
		EXPECT_TRUE(std::holds_alternative<const enum_underlying_type*>(ptr));
		const auto& eptr = std::get<const enum_underlying_type*>(ptr);
		EXPECT_EQ(reinterpret_cast<const Enum*>(eptr), &src);
	}
	{
		enum Enum
		{
			FIRST,
			SECOND
		};
		std::optional<Enum> src;
		parameter           x{ src, parameter::by_reference{} };
		EXPECT_TRUE(std::holds_alternative<parameter::reference_type>(x.value()));
		const auto& ref = std::get<parameter::reference_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<parameter::pointer_optional_type>(ref));
		const auto& ptr            = std::get<parameter::pointer_optional_type>(ref);
		using enum_underlying_type = std::underlying_type_t<Enum>;
		EXPECT_TRUE(std::holds_alternative<const std::optional<enum_underlying_type>*>(ptr));
		const auto& eptr = std::get<const std::optional<enum_underlying_type>*>(ptr);
		EXPECT_EQ(reinterpret_cast<const std::optional<Enum>*>(eptr), &src);
	}
	{
		enum class Enum : char
		{
			FIRST  = 'A',
			SECOND = 'B'
		} src{};
		parameter x{ src, parameter::by_reference{} };
		EXPECT_TRUE(std::holds_alternative<parameter::reference_type>(x.value()));
		const auto& ref = std::get<parameter::reference_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<parameter::pointer_type>(ref));
		const auto& ptr = std::get<parameter::pointer_type>(ref);
		EXPECT_TRUE(std::holds_alternative<const char*>(ptr));
		const auto& eptr = std::get<const char*>(ptr);
		EXPECT_EQ(reinterpret_cast<const Enum*>(eptr), &src);
	}
	{
		enum class Enum : char
		{
			FIRST  = 'A',
			SECOND = 'B'
		};
		std::optional<Enum> src;
		parameter           x{ src, parameter::by_reference{} };
		EXPECT_TRUE(std::holds_alternative<parameter::reference_type>(x.value()));
		const auto& ref = std::get<parameter::reference_type>(x.value());
		EXPECT_TRUE(std::holds_alternative<parameter::pointer_optional_type>(ref));
		const auto& ptr = std::get<parameter::pointer_optional_type>(ref);
		EXPECT_TRUE(std::holds_alternative<const std::optional<char>*>(ptr));
		const auto& eptr = std::get<const std::optional<char>*>(ptr);
		EXPECT_EQ(reinterpret_cast<const std::optional<Enum>*>(eptr), &src);
	}
}

TEST(ParameterTest, ByReferenceConstructFromStdOptional)
{
	std::optional<int> src{ 42 };
	parameter          x{ src, parameter::by_reference{} };
	EXPECT_TRUE(std::holds_alternative<parameter::reference_type>(x.value()));
	const auto& ref = std::get<parameter::reference_type>(x.value());
	EXPECT_TRUE(std::holds_alternative<parameter::pointer_optional_type>(ref));
	const auto& ptr = std::get<parameter::pointer_optional_type>(ref);
	EXPECT_TRUE(std::holds_alternative<const std::optional<int>*>(ptr));
	const auto& eptr = std::get<const std::optional<int>*>(ptr);
	EXPECT_EQ(eptr, &src);
}

TEST(ParameterTest, TestPointerGetter)
{
#define TEST_POINTER_GETTER(TYPE, VALUE)                                                                                                   \
	do                                                                                                                                     \
	{                                                                                                                                      \
		{                                                                                                                                  \
			TYPE       src{ VALUE };                                                                                                       \
			parameter  x{ src, parameter::by_value{} };                                                                                    \
			const auto ptr = x.pointer();                                                                                                  \
			EXPECT_TRUE(std::holds_alternative<const TYPE*>(ptr));                                                                         \
			const auto& eptr = std::get<const TYPE*>(ptr);                                                                                 \
			EXPECT_NE(eptr, nullptr);                                                                                                      \
			EXPECT_NE(eptr, &src);                                                                                                         \
			EXPECT_EQ(*eptr, src);                                                                                                         \
		}                                                                                                                                  \
                                                                                                                                           \
		{                                                                                                                                  \
			std::optional<TYPE> src{ std::nullopt };                                                                                       \
			parameter           x{ src, parameter::by_value{} };                                                                           \
			const auto          ptr = x.pointer();                                                                                         \
			EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));                                                               \
		}                                                                                                                                  \
                                                                                                                                           \
		{                                                                                                                                  \
			TYPE      src{ VALUE };                                                                                                        \
			parameter x{ src, parameter::by_reference{} };                                                                                 \
			{                                                                                                                              \
				const auto ptr = x.pointer();                                                                                              \
				EXPECT_TRUE(std::holds_alternative<const TYPE*>(ptr));                                                                     \
				const auto& eptr = std::get<const TYPE*>(ptr);                                                                             \
				EXPECT_EQ(eptr, &src);                                                                                                     \
			}                                                                                                                              \
			{                                                                                                                              \
				TYPE deflt{};                                                                                                              \
				src            = deflt;                                                                                                    \
				const auto ptr = x.pointer();                                                                                              \
				EXPECT_TRUE(std::holds_alternative<const TYPE*>(ptr));                                                                     \
				const auto& eptr = std::get<const TYPE*>(ptr);                                                                             \
				EXPECT_EQ(eptr, &src);                                                                                                     \
			}                                                                                                                              \
		}                                                                                                                                  \
                                                                                                                                           \
		{                                                                                                                                  \
			std::optional<TYPE> src{ VALUE };                                                                                              \
			parameter           x{ src, parameter::by_reference{} };                                                                       \
			{                                                                                                                              \
				const auto ptr = x.pointer();                                                                                              \
				EXPECT_TRUE(std::holds_alternative<const TYPE*>(ptr));                                                                     \
				const auto& eptr = std::get<const TYPE*>(ptr);                                                                             \
				EXPECT_EQ(eptr, &src.value());                                                                                             \
			}                                                                                                                              \
			{                                                                                                                              \
				TYPE deflt{};                                                                                                              \
				src            = deflt;                                                                                                    \
				const auto ptr = x.pointer();                                                                                              \
				EXPECT_TRUE(std::holds_alternative<const TYPE*>(ptr));                                                                     \
				const auto& eptr = std::get<const TYPE*>(ptr);                                                                             \
				EXPECT_EQ(eptr, &src.value());                                                                                             \
			}                                                                                                                              \
			{                                                                                                                              \
				src            = std::nullopt;                                                                                             \
				const auto ptr = x.pointer();                                                                                              \
				EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));                                                           \
			}                                                                                                                              \
		}                                                                                                                                  \
                                                                                                                                           \
		{                                                                                                                                  \
			std::optional<TYPE> src{ std::nullopt };                                                                                       \
			parameter           x{ src, parameter::by_reference{} };                                                                       \
			{                                                                                                                              \
				const auto ptr = x.pointer();                                                                                              \
				EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));                                                           \
			}                                                                                                                              \
			{                                                                                                                              \
				src            = VALUE;                                                                                                    \
				const auto ptr = x.pointer();                                                                                              \
				EXPECT_TRUE(std::holds_alternative<const TYPE*>(ptr));                                                                     \
				const auto& eptr = std::get<const TYPE*>(ptr);                                                                             \
				EXPECT_EQ(eptr, &src.value());                                                                                             \
			}                                                                                                                              \
		}                                                                                                                                  \
	} while (false)

	byte_string_view bsv{ reinterpret_cast<const unsigned char*>("foo"), 3 };

	TEST_POINTER_GETTER(bool, true);
	TEST_POINTER_GETTER(char, 'A');
	TEST_POINTER_GETTER(signed char, 42);
	TEST_POINTER_GETTER(unsigned char, 42);
	TEST_POINTER_GETTER(std::int16_t, 42);
	TEST_POINTER_GETTER(std::uint16_t, 42);
	TEST_POINTER_GETTER(std::int32_t, 42);
	TEST_POINTER_GETTER(std::uint32_t, 42);
	TEST_POINTER_GETTER(std::int64_t, 42);
	TEST_POINTER_GETTER(std::uint64_t, 42);
	TEST_POINTER_GETTER(float, 42);
	TEST_POINTER_GETTER(double, 42);
	TEST_POINTER_GETTER(long double, 42);
	TEST_POINTER_GETTER(std::string_view, "42");
	TEST_POINTER_GETTER(std::string, "42");
	TEST_POINTER_GETTER(byte_string_view, bsv);
	TEST_POINTER_GETTER(byte_string, bsv);
	TEST_POINTER_GETTER(time_point, std::chrono::system_clock::now());
	// These do not have an operator== :(
	//TEST_POINTER_GETTER(date, std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
	//TEST_POINTER_GETTER(time_of_day, std::chrono::hours{ 12 });
}

TEST(ParameterTest, TestPointerGetterEnum)
{
	enum Enum
	{
		FIRST,
		SECOND
	};
	using base = std::underlying_type_t<Enum>;

	{
		Enum       src{ FIRST };
		parameter  x{ src, parameter::by_value{} };
		const auto ptr = x.pointer();
		EXPECT_TRUE(std::holds_alternative<const base*>(ptr));
		const auto& eptr = std::get<const base*>(ptr);
		EXPECT_NE(eptr, nullptr);
		EXPECT_NE((void*)(eptr), (void*)(&src));
		EXPECT_EQ(*eptr, src);
	}

	{
		std::optional<Enum> src{ std::nullopt };
		parameter           x{ src, parameter::by_value{} };
		const auto          ptr = x.pointer();
		EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));
	}

	{
		Enum      src{ FIRST };
		parameter x{ src, parameter::by_reference{} };
		{
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const base*>(ptr));
			const auto& eptr = std::get<const base*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src));
		}
		{
			src            = SECOND;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const base*>(ptr));
			const auto& eptr = std::get<const base*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src));
		}
	}

	{
		std::optional<Enum> src{ FIRST };
		parameter           x{ src, parameter::by_reference{} };
		{
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const base*>(ptr));
			const auto& eptr = std::get<const base*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src.value()));
		}
		{
			src            = SECOND;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const base*>(ptr));
			const auto& eptr = std::get<const base*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src.value()));
		}
		{
			src            = std::nullopt;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));
		}
	}

	{
		std::optional<Enum> src{ std::nullopt };
		parameter           x{ src, parameter::by_reference{} };
		{
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));
		}
		{
			src            = FIRST;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const base*>(ptr));
			const auto& eptr = std::get<const base*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src.value()));
		}
	}
}

TEST(ParameterTest, TestPointerGetterEnumChar)
{
	enum class Enum : char
	{
		FIRST  = 'A',
		SECOND = 'B'
	};

	{
		Enum       src{ Enum::FIRST };
		parameter  x{ src, parameter::by_value{} };
		const auto ptr = x.pointer();
		EXPECT_TRUE(std::holds_alternative<const char*>(ptr));
		const auto& eptr = std::get<const char*>(ptr);
		EXPECT_NE(eptr, nullptr);
		EXPECT_NE((void*)(eptr), (void*)(&src));
		EXPECT_EQ(*eptr, (char)src);
	}

	{
		std::optional<Enum> src{ std::nullopt };
		parameter           x{ src, parameter::by_value{} };
		const auto          ptr = x.pointer();
		EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));
	}

	{
		Enum      src{ Enum::FIRST };
		parameter x{ src, parameter::by_reference{} };
		{
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const char*>(ptr));
			const auto& eptr = std::get<const char*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src));
		}
		{
			src            = Enum::SECOND;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const char*>(ptr));
			const auto& eptr = std::get<const char*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src));
		}
	}

	{
		std::optional<Enum> src{ Enum::FIRST };
		parameter           x{ src, parameter::by_reference{} };
		{
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const char*>(ptr));
			const auto& eptr = std::get<const char*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src.value()));
		}
		{
			src            = Enum::SECOND;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const char*>(ptr));
			const auto& eptr = std::get<const char*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src.value()));
		}
		{
			src            = std::nullopt;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));
		}
	}

	{
		std::optional<Enum> src{ std::nullopt };
		parameter           x{ src, parameter::by_reference{} };
		{
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const std::nullopt_t*>(ptr));
		}
		{
			src            = Enum::FIRST;
			const auto ptr = x.pointer();
			EXPECT_TRUE(std::holds_alternative<const char*>(ptr));
			const auto& eptr = std::get<const char*>(ptr);
			EXPECT_EQ((void*)(eptr), (void*)(&src.value()));
		}
	}
}

} // namespace squid
