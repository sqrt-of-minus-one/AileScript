#pragma once

#include <string>
#include <type_traits>

namespace enc
{

// A few constants for different UTF encodings
constexpr char32_t U8_1_LIMIT = 0x7FF;
constexpr char32_t U8_2_LIMIT = 0x7FFF;
constexpr char32_t U8_3_LIMIT = 0xFFFF;
constexpr char32_t U8_4_LIMIT = 0x10FFFF;
constexpr char8_t U8_1_1_BASE = 0b0000'0000;
constexpr char8_t U8_1_1_MASK = 0b0111'1111;
constexpr char8_t U8_2_1_BASE = 0b1100'0000;
constexpr char8_t U8_2_1_MASK = 0b0001'1111;
constexpr char8_t U8_3_1_BASE = 0b1110'0000;
constexpr char8_t U8_3_1_MASK = 0b0000'1111;
constexpr char8_t U8_4_1_BASE = 0b1111'0000;
constexpr char8_t U8_4_1_MASK = 0b0000'0111;
constexpr char8_t U8_N_N_BASE = 0b1000'0000;
constexpr char8_t U8_N_N_MASK = 0b0011'1111;
constexpr int U8_N_N_DIGITS = 6;

constexpr char32_t U16_1_LIMIT = 0xD7FF;
constexpr char32_t U16_2_LIMIT = 0x10FFFF;
constexpr char16_t U16_1_1_BASE = 0b0000'0000'0000'0000;
constexpr char16_t U16_1_1_MASK = 0b0000'0011'1111'1111;
constexpr char16_t U16_2_1_BASE = 0b1101'1000'0000'0000;
constexpr char16_t U16_2_1_MASK = 0b0000'0011'1111'1111;
constexpr char16_t U16_2_2_BASE = 0b1101'1100'0000'0000;
constexpr char16_t U16_2_2_MASK = 0b0000'0011'1111'1111;
constexpr int U16_2_2_DIGITS = 10;

template<typename T>
concept String =
	std::is_same_v<T, std::string> ||
	std::is_same_v<T, std::u8string> ||
	std::is_same_v<T, std::u16string> ||
	std::is_same_v<T, std::u32string>;

// Convert between different UTF encodings

template<String T>
std::string to_def(const T& str);

template<String T>
std::u8string to_8(const T& str);

template<String T>
std::u16string to_16(const T& str);

template<String T>
std::u32string to_32(const T& str) = delete;

template<>
std::u32string to_32<std::string>(const std::string& str);

template<>
std::u32string to_32<std::u8string>(const std::u8string& str);

template<>
std::u32string to_32<std::u16string>(const std::u16string& str);

template<>
std::u32string to_32<std::u32string>(const std::u32string& str);

}

std::ostream& operator<<(std::ostream& stream, const std::u8string& string);
std::ostream& operator<<(std::ostream& stream, const std::u16string& string);
std::ostream& operator<<(std::ostream& stream, const std::u32string& string);
std::ostream& operator<<(std::ostream& stream, const char8_t* string);
std::ostream& operator<<(std::ostream& stream, const char16_t* string);
std::ostream& operator<<(std::ostream& stream, const char32_t* string);

template<enc::String T>
std::string enc::to_def(const T& str)
{
	if constexpr (std::is_same_v<T, std::string>)
	{
		return str;
	}
	
	// For Linux or anyone else who uses UTF-8 by default
	// Maybe I'll write something for Windows later
	std::u8string str8 = to_8(str); // Convert to UTF-8
	std::string str_def;
	for (char8_t c : str8)
	{
		// Just copy every character
		str_def.push_back(c);
	}
	return str_def;
}

template<enc::String T>
std::u8string enc::to_8(const T& str)
{
	if constexpr (std::is_same_v<T, std::u8string>)
	{
		return str;
	}

	std::u32string str32 = to_32(str); // Convert to UTF-32 first
	std::u8string str8;
	for (char32_t c : str32)
	{
		if (c <= U8_1_LIMIT) // One byte for the character
		{
			char8_t c1 = c;
			str8.push_back(c1);
		}
		else if (c <= U8_2_LIMIT) // Two bytes for the character
		{
			char8_t c2 = U8_N_N_BASE | c & U8_N_N_MASK;
			c >>= U8_N_N_DIGITS;
			char8_t c1 = U8_2_1_BASE | c & U8_2_1_MASK;
			str8.push_back(c1);
			str8.push_back(c2);
		}
		else if (c <= U8_3_LIMIT) // Three bytes for the character
		{
			char8_t c3 = U8_N_N_BASE | c & U8_N_N_MASK;
			c >>= U8_N_N_DIGITS;
			char8_t c2 = U8_N_N_BASE | c & U8_N_N_MASK;
			c >>= U8_N_N_DIGITS;
			char8_t c1 = U8_3_1_BASE | c & U8_3_1_MASK;
			str8.push_back(c1);
			str8.push_back(c2);
			str8.push_back(c3);
		}
		else if (c <= U8_4_LIMIT) // Four bytes for the character
		{
			char8_t c4 = U8_N_N_BASE | c & U8_N_N_MASK;
			c >>= U8_N_N_DIGITS;
			char8_t c3 = U8_N_N_BASE | c & U8_N_N_MASK;
			c >>= U8_N_N_DIGITS;
			char8_t c2 = U8_N_N_BASE | c & U8_N_N_MASK;
			c >>= U8_N_N_DIGITS;
			char8_t c1 = U8_4_1_BASE | c & U8_4_1_MASK;
			str8.push_back(c1);
			str8.push_back(c2);
			str8.push_back(c3);
			str8.push_back(c4);
		}
	}
	return str8;
}

template<enc::String T>
std::u16string enc::to_16(const T& str)
{
	if constexpr (std::is_same_v<T, std::u16string>)
	{
		return str;
	}

	std::u32string str32 = to_32(str); // Convert to UTF-32 first
	std::u16string str16;
	for (char32_t c : str32)
	{
		if (c <= U16_1_LIMIT) // One two-byte word for the character
		{
			char16_t c1 = c;
			str16.push_back(c1);
		}
		else if (c <= U16_2_LIMIT) // Two two-byte words for the character
		{
			char16_t c2 = U16_2_2_BASE | c & U16_2_2_MASK;
			c >>= 10;
			char16_t c1 = U16_2_1_BASE | c & U16_2_1_MASK;
			str16.push_back(c1);
			str16.push_back(c2);
		}
	}
	return str16;
}
