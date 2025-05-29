#pragma once

#include "Token.h"

#include <list>
#include <stack>

class Parser
{
private:
	enum class EWaitingStatus
	{
		NONE = 0, // No incomplete tokens: the next charater will start a new token
		WORD, // A word (keyword or id) has been started
		PUNCTUATOR, // A punctuator has been started
		DOT, // A dot may be either a punctuator or a part of a float literal

		INT_LITERAL, // A number has been started
		ZERO_INT_LITERAL, // A single zero has been received (a zero may be the part of an integer prefix like 0x)
		HEX_INT_LITERAL, // A number with a hexadecimal prefix has been started
		OCT_INT_LITERAL, // A number with an octal prefix has been started
		ZERO_OCT_INT_LITERAL, // A number has been started with 0 (which means it's octal)
		BIN_INT_LITERAL, // A number with a binary prefix has been started
		INT_LITERAL_SUFFIX, // An integer value has ended, its suffix/postfix has been started

		FLOAT_LITERAL, // A float literal has been started (there has been a point)
		FLOAT_LITERAL_EXP_SIGN, // The exponential part of a float literal has been just started: a sign is possible
		FLOAT_LITERAL_EXP, // The exponential part of a float literal has been started (a sign is not possible anymore)
		FLOAT_LITERAL_SUFFIX, // A float value has ended, its suffix/postfix has been started

		CHAR_LITERAL, // A character literal has been started (there has been a ' character)
		CHAR_LITERAL_SUFFIX, // A character value has ended, its postfix has been started

		STRING_LITERAL, // A string literal has been started (there has been a " character)
		STRING_LITERAL_SUFFIX, // A string value has ended, its postfix has been started

		SINGLE_LINE_COMMENT, // A single-line comment has been started: everything until the end of line shall be ignored
		SINGLE_LINE_COMMENT_BACKSLASH, // A backslash has been received inside a signle-line comment: if the next character is a new line, the comment won't stop
		MULTI_LINE_COMMENT, // A multi-line comment has been started: everything until the */ shall be ignored
		MULTI_LINE_COMMENT_ENDING // A multi-line comment has been started and may be about to end: there has been a * character, but not / yet
	};

	enum class EWaitingEscape
	{
		NONE = 0, // No escape sequence has been initiated
		PLAIN, // There has been a backslash: an escape sequence has been initiated
		U16, // There has been a backslash and 'u'/'ю' character: 4 hexadecimal digits or '{' is being expected
		U16_STARTED, // There has been a backslach, 'u'/'ю' character, and hexadecimal digets, more digits are being expected
		U32, // There has been a backslash and 'U'/'Ю' character: 8 hexadecimal digits or '{' is being expected
		U32_STARTED, // There has been a backslach, 'U'/'Ю' character, and hexadecimal digets, more digits are being expected
		U_BRACKETS // There has been a backslash, 'u'/'ю' or 'U'/'Ю', and '{' characters; hexadecimal digits or '}' is beign expected
	};

	std::u16string waiting_; // The characters waiting to be analysed
	EWaitingStatus waiting_status_ = EWaitingStatus::NONE; // What kind of data is stored in the waiting_ string and what is expected next
	
	EWaitingEscape waiting_escape_ = EWaitingEscape::NONE; // What kind of escape sequence (if any) has been initiated
	uint32_t escape_code_ = 0; // A variable for storing a Unicode code of an escape sequence
	int escape_chars_counter_ = 0; // How many hexadecimal digits have been stored in the escape_code

	TokenList tokens_; // Parsed tokens

public:
	void next(char16_t c); // Put a next character
	TokenList extract_tokens();

private:
	void next_escape_char_(char16_t c); // Next hexadecimal digit of an escape sequence
	void apply_escape_(); // An escape sequence containing a Unicode character code is completed
	void next_(char16_t c, bool escaped = false); // Put a next character

	void new_token_(char16_t c); // Start a new token with the character

	// The token is complete, parse it:
	void parse_word_();
	void parse_punctuator_(bool is_end); // A punctuator may not be complete
	void parse_int_literal();
	void parse_float_literal();
	void parse_char_literal();
	void parse_string_literal();
};

constexpr bool is_space(char16_t c)
{
	constexpr std::u16string_view CHARS = u" \t\n\f\v\r";
	return CHARS.find(c) != CHARS.npos;
}

constexpr bool is_punctuator(char16_t c)
{
	constexpr std::u16string_view CHARS = u"!\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~";
	return CHARS.find(c) != CHARS.npos;
}

constexpr bool is_digit(char16_t c)
{
	return c >= u'0' && c <= u'9';
}

constexpr bool is_bin_digit(char16_t c)
{
	return c == u'0' || c == u'1';
}

constexpr bool is_oct_digit(char16_t c)
{
	return c >= u'0' && c <= u'7';
}

constexpr bool is_hex_digit(char16_t c)
{
	return
		c >= u'0' && c <= u'9' ||
		c >= u'A' && c <= u'F' ||
		c >= u'a' && c <= u'f';
}

constexpr bool is_int_prefix(char16_t c)
{
	constexpr std::u16string_view CHARS = u"xXhHoObBшШоОбБ";
	return CHARS.find(c) != CHARS.npos;
}
