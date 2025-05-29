#include "encoding.h"

template<>
std::u32string enc::to_32<std::string>(const std::string& str)
{
	std::u8string str8;
	for (char c : str)
	{
		str8.push_back(c);
	}
	return to_32(str8);
}

template<>
std::u32string enc::to_32<std::u8string>(const std::u8string& str)
{
	std::u32string str32;
	for (int i = 0; i < str.length(); ++i)
	{
		if ((str[i] & ~U8_1_1_MASK) == U8_1_1_BASE) // A single-byte character
		{
			char32_t c = str[i];
			str32.push_back(c);
		}
		else if ((str[i] & ~U8_2_1_MASK) == U8_2_1_BASE && // A two-byte chacater
			(str[i + 1] & ~U8_N_N_MASK) == U8_N_N_BASE)
		{
			char32_t c = str[i] & U8_2_1_MASK;
			c <<= U8_N_N_DIGITS;
			c |= str[i + 1] & U8_N_N_MASK;
			str32.push_back(c);
		}
		else if ((str[i] & ~U8_3_1_MASK) == U8_3_1_BASE && // A two-byte chacater
			(str[i + 1] & ~U8_N_N_MASK) == U8_N_N_BASE &&
			(str[i + 2] & ~U8_N_N_MASK) == U8_N_N_BASE)
		{
			char32_t c = str[i] & U8_3_1_MASK;
			c <<= U8_N_N_DIGITS;
			c |= str[i + 1] & U8_N_N_MASK;
			c <<= U8_N_N_DIGITS;
			c |= str[i + 2] & U8_N_N_MASK;
			str32.push_back(c);
		}
		else if ((str[i] & ~U8_4_1_MASK) == U8_4_1_BASE && // A two-byte chacater
			(str[i + 1] & ~U8_N_N_MASK) == U8_N_N_BASE &&
			(str[i + 2] & ~U8_N_N_MASK) == U8_N_N_BASE &&
			(str[i + 3] & ~U8_N_N_MASK) == U8_N_N_BASE)
		{
			char32_t c = str[i] & U8_4_1_MASK;
			c <<= U8_N_N_DIGITS;
			c |= str[i + 1] & U8_N_N_MASK;
			c <<= U8_N_N_DIGITS;
			c |= str[i + 2] & U8_N_N_MASK;
			c <<= U8_N_N_DIGITS;
			c |= str[i + 3] & U8_N_N_MASK;
			str32.push_back(c);
		}
		// else - ignore an invalid character
	}
	return str32;
}

template<>
std::u32string enc::to_32<std::u16string>(const std::u16string& str)
{
	std::u32string str32;
	for (int i = 0; i < str.length(); ++i)
	{
		if (str[i] <= U16_1_LIMIT) // A single-word character
		{
			char32_t c = str[i];
			str32.push_back(c);
		}
		else if ((str[i] & ~U16_2_1_MASK) == U16_2_1_BASE && // A two-word character
			(str[i + 1] & ~U16_2_2_MASK) == U16_2_2_BASE)
		{
			char32_t c = str[i] & U16_2_1_MASK;
			c <<= U16_2_2_DIGITS;
			c |= str[i + 1] & U16_2_2_MASK;
			str32.push_back(c);
		}
		// else - ignore an invalid character
	}
	return str32;
}

template<>
std::u32string enc::to_32<std::u32string>(const std::u32string& str)
{
	return str;
}

std::ostream& operator<<(std::ostream& stream, const std::u8string& string)
{
	return stream << enc::to_def(string);
}

std::ostream& operator<<(std::ostream& stream, const std::u16string& string)
{
	return stream << enc::to_def(string);
}

std::ostream& operator<<(std::ostream& stream, const std::u32string& string)
{
	return stream << enc::to_def(string);
}

std::ostream& operator<<(std::ostream& stream, const char8_t* string)
{
	return stream << std::u8string(string);
}

std::ostream& operator<<(std::ostream& stream, const char16_t* string)
{
	return stream << std::u16string(string);
}

std::ostream& operator<<(std::ostream& stream, const char32_t* string)
{
	return stream << std::u32string(string);
}
