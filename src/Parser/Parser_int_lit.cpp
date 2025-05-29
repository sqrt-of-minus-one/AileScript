#include "Parser.h"

void Parser::parse_int_literal()
{
	if (waiting_.empty())
	{
		return;
	}

	std::shared_ptr<IntLiteralToken> token = std::make_shared<IntLiteralToken>();
	token->prefix = IntLiteralToken::EPrefix::NONE;
	token->suffix = IntLiteralToken::ESuffix::NONE;

	// Indices of the first characters of the value, the suffix and the postfix
	int value_start = 0, suffix_start = 0, postfix_start = 0;

	if (waiting_.length() > 1 && waiting_[0] == u'0') // Any prefix starts with 0
	{
		switch (waiting_[1])
		{
		case u'x':
		case u'X':
		case u'h':
		case u'H':
		case u'ш':
		case u'Ш':
		{
			token->prefix = IntLiteralToken::EPrefix::HEX;
			value_start = 2;
			break;
		}
		case u'o':
		case u'O':
		case u'о':
		case u'О':
		{
			token->prefix = IntLiteralToken::EPrefix::OCT;
			value_start = 2;
			break;
		}
		case u'b':
		case u'B':
		case u'д':
		case u'Д':
		case u'б':
		case u'Б':
		{
			token->prefix = IntLiteralToken::EPrefix::BIN;
			value_start = 2;
			break;
		}
		default:
		{
			token->prefix = IntLiteralToken::EPrefix::OCT;
			value_start = 1;
		}
		}
	}

	// Neither suffix nor postfix can start with hex digit, so it's safe to use the is_hex_digit() condition
	for (suffix_start = 0;
		is_hex_digit(waiting_[suffix_start]) || waiting_[suffix_start] == u'\''; // The last character is null, which breaks from the loop
		++suffix_start)
		;
	// If there is no suffix/prefix, suffix_start is waiting_.length()
	
	postfix_start = suffix_start; // If there is no suffix
	switch (waiting_[suffix_start]) // Whether there is a suffix
	{
	case u'y':
	case u'Y':
	{
		// byte suffix
		token->suffix = IntLiteralToken::ESuffix::BYTE;
		postfix_start = suffix_start + 1;
		break;
	}
	case u's':
	case u'S':
	case u'к':
	case u'К':
	{
		// short suffix
		token->suffix = IntLiteralToken::ESuffix::SHORT;
		postfix_start = suffix_start + 1;
		break;
	}
	case u'l':
	case u'L':
	case u'д':
	case u'Д':
	{
		// long suffix
		token->suffix = IntLiteralToken::ESuffix::LONG;
		postfix_start = suffix_start + 1;
		break;
	}
	case u'u':
	case u'U':
	{
		switch (waiting_[suffix_start + 1])
		{
		case u'y':
		case u'Y':
		{
			// ubyte suffix
			token->suffix = IntLiteralToken::ESuffix::UBYTE;
			postfix_start = suffix_start + 2;
			break;
		}
		case u's':
		case u'S':
		{
			// ushort suffix
			token->suffix = IntLiteralToken::ESuffix::USHORT;
			postfix_start = suffix_start + 2;
			break;
		}
		case u'l':
		case u'L':
		{
			// ulong suffix
			token->suffix = IntLiteralToken::ESuffix::ULONG;
			postfix_start = suffix_start + 2;
			break;
		}
		default:
		{
			// uint suffix
			token->suffix = IntLiteralToken::ESuffix::UINT;
			postfix_start = suffix_start + 1;
		}
		}
		break;
	}
	case u'б':
	case u'Б':
	{
		switch (waiting_[suffix_start + 1])
		{
		case u'б':
		case u'Б':
		{
			// ubyte suffix
			token->suffix = IntLiteralToken::ESuffix::UBYTE;
			postfix_start = suffix_start + 2;
			break;
		}
		case u'к':
		case u'К':
		{
			// ushort suffix
			token->suffix = IntLiteralToken::ESuffix::USHORT;
			postfix_start = suffix_start + 2;
			break;
		}
		case u'д':
		case u'Д':
		{
			// ulong suffix
			token->suffix = IntLiteralToken::ESuffix::ULONG;
			postfix_start = suffix_start + 2;
			break;
		}
		case u'ц':
		case u'Ц':
		{
			// uint suffix
			token->suffix = IntLiteralToken::ESuffix::UINT;
			postfix_start = suffix_start + 2;
		}
		default:
		{
			// byte suffix
			token->suffix = IntLiteralToken::ESuffix::BYTE;
			postfix_start = suffix_start + 1;
		}
		}
		break;
	}
	}
	
	// Get value and postfix
	token->value = waiting_.substr(value_start, suffix_start - value_start);
	token->postfix = waiting_.substr(postfix_start);
	tokens_.push_back(token);

	waiting_.clear();
	waiting_status_ = EWaitingStatus::NONE;
}
