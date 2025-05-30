#pragma once

#include <string>
#include <memory>
#include <optional>
#include <list>
#include <iostream>

struct Token
{
	using Ptr = std::shared_ptr<Token>;

	enum class EType
	{
		INVALID = 0,
		GROUP,
		ID, KEYWORD, PUNCTUATOR,
		INT_LITERAL, FLOAT_LITERAL, CHAR_LITERAL, STRING_LITERAL
	};

	virtual EType get_type() const = 0;
	virtual void print() const = 0;
};
using TokenList = std::list<Token::Ptr>;

// The presence of an invalid token in a program is an error
struct InvalidToken : public Token
{
	enum class EInvalidType
	{
		UNKNOWN = 0,
		ESCAPE // Invalid escape sequence
	};

	virtual EType get_type() const override
	{
		return EType::INVALID;
	}

	EInvalidType invalid_type = EInvalidType::UNKNOWN;

	virtual void print() const override;
};

// Group of tokens surrounded by pair characters
struct GroupToken : public Token
{
	// Characters surrounding the group
	enum class EGroupType
	{
		PARENTHESIS,	// ()
		SQUARE_BRACKET,	// []
		BRACKET			// {}
	};

	virtual EType get_type() const override
	{
		return EType::GROUP;
	}

	EGroupType group_type;
	TokenList tokens; // The list of tokens in the group

	virtual void print() const override;
};

// Identifier (names of variables, functions, user types etc.)
struct IdToken : public Token
{
	virtual EType get_type() const override
	{
		return EType::ID;
	}

	std::u16string name; // The identifier

	virtual void print() const override;
};

struct KeywordToken : public Token
{
	enum class EKeyword
	{
		// Types
		BYTE, SHORT, INT, LONG,
		UBYTE, USHORT, UINT, ULONG,
		BOOL, FLOAT, DOUBLE, LDOUBLE,
		CHAR, CHAR8, CHAR16, CHAR32,
		FNC, AUTO,

		// Conditionals and loops
		IF, ELSE,
		FOR, WHILE, DO
	};

	virtual EType get_type() const override
	{
		return EType::KEYWORD;
	}

	EKeyword keyword;

	virtual void print() const override;
};

struct PunctuatorToken : public Token
{
	// Some punctuators below (like " ' // /*) cannot appear in a token list:
	enum class EPunctuator
	{
		NOT, NOT_EQ, // ! !=
		QUOTE, // "
		CONST, CONSTEXPR, // # ##
		DOLLAR, // $
		MOD, MOD_AS, // % %=
		BIT_AND, AND, AND_AS, BIT_AND_AS, // & && &&= &=
		SINGLE_QUOTE, // '
		PAR_LEFT, PAR_RIGHT, // ( )
		MUL, COMMENT_END, MUL_AS, // * */ *=
		PLUS, INC, PLUS_AS, // + ++ +=
		COMMA, // ,
		MINUS, DEC, MINUS_AS, PTR_MEMBER, // - -- -= ->
		DOT, RANGE, ELLIPSIS, // . .. ...
		DIV, COMMENT_START, COMMENT, INT_DIV, INT_DIV_AS, DIV_AS, // / /* // /: /:= /=
		COLON, SCOPE, // : ::
		SEMICOLON, // ;
		LESS, SHIFT_LEFT, SHIFT_LEFT_AS, LESS_EQ, SPACESHIP, STATIC_CAST, DYNAMIC_CAST, CONST_CAST, REINTERPRET_CAST, // < << <<= <= <=> <| <~| <#| <?|
		ASSIGN, EQUAL, RETURN, // = == =>
		GREATER, GREATER_EQ, SHIFT_RIGHT, SHIFT_RIGHT_AS, // > >= >> >>=
		QUESTION, // ?
		AT, // @
		SQ_BRACKET_LEFT, BACKSLASH, SQ_BRACKET_RIGHT, // [ \ ]
		XOR, XOR_AS, POW, POW_AS, // ^ ^= ^^ ^^=
		APOSTROPHE, // `
		BRACKET_LEFT, // {
		BIT_OR, BIT_OR_AS, OR, OR_AS, // | |= || ||=
		BRACKET_RIGHT, // }
		COMPL, COMPL_AS // ~ ~=
	};

	virtual EType get_type() const override
	{
		return EType::PUNCTUATOR;
	}

	EPunctuator punctuator;
	std::optional<TokenList::const_iterator> pair; // A pair for parentheses, brackets etc.

	virtual void print() const override;
};

// Literals can have:
//	prefix: before the literal value
//	suffix: after the literal value
//	postfix: after the suffix
// Postfixes are user-defined, the list of prefixes and suffixes is fixed
// E.g. 0x123l_s
//	0x is a prefix, 123 is a literal value, l is a suffix, _s is a postfix

// Integer literal
struct IntLiteralToken : public Token
{
	enum class EPrefix
	{
		NONE = 0,
		HEX, OCT, BIN
	};
	enum class ESuffix
	{
		NONE = 0,
		BYTE, SHORT, LONG,
		UBYTE, USHORT, UINT, ULONG
	};

	virtual EType get_type() const override
	{
		return EType::INT_LITERAL;
	}

	EPrefix prefix = EPrefix::NONE;
	
	// The sign is not included in the value: it's treated as an operator
	std::u16string value;
	ESuffix suffix = ESuffix::NONE;
	std::u16string postfix;

	virtual void print() const override;
};

struct FloatLiteralToken : public Token
{
	enum class ESuffix
	{
		NONE = 0,
		FLOAT, LDOUBLE
	};
	enum class ESign
	{
		MINUS, PLUS
	};

	virtual EType get_type() const override
	{
		return EType::FLOAT_LITERAL;
	}

	// The sign is not included in the value: it's treated as an operator
	std::u16string int_part, real_part, exp_part;
	ESign exp_sign = ESign::PLUS; // The sign of the exponent part
	ESuffix suffix = ESuffix::NONE;
	std::u16string postfix;

	virtual void print() const override;
};

struct CharLiteralToken : public Token
{
	enum class EPrefix
	{
		INVALID = -1,
		NONE = 0,
		U8, U16, U32
	};

	virtual EType get_type() const override
	{
		return EType::CHAR_LITERAL;
	}

	EPrefix prefix = EPrefix::NONE;
	std::u16string value, postfix;

	virtual void print() const override;
};

struct StringLiteralToken : public Token
{
	enum class EPrefix
	{
		INVALID = -1,
		NONE = 0,
		U8, U16, U32
	};

	virtual EType get_type() const override
	{
		return EType::STRING_LITERAL;
	}

	EPrefix prefix = EPrefix::NONE;
	std::u16string value, postfix;

	virtual void print() const override;
};
