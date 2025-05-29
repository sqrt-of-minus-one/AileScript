#include "Parser.h"

#include <limits>

void Parser::next(char16_t c)
{
	switch (waiting_escape_)
	{
	case EWaitingEscape::NONE: // The escape sequence hasn't been initiated
	{
		if (c != u'\\' ||
			waiting_status_ == EWaitingStatus::SINGLE_LINE_COMMENT ||
			waiting_status_ == EWaitingStatus::SINGLE_LINE_COMMENT_BACKSLASH ||
			waiting_status_ == EWaitingStatus::MULTI_LINE_COMMENT ||
			waiting_status_ == EWaitingStatus::MULTI_LINE_COMMENT_ENDING)
		{
			next_(c);
		}
		else
		{
			// Initiate an escape sequence if the character is a backslash (but not in a comment)
			waiting_escape_ = EWaitingEscape::PLAIN;
		}
		break;
	}
	case EWaitingEscape::PLAIN: // A backslash has been received
	{
		switch (c)
		{
		// Simple escape characters
		case u'0':
		{
			next_(u'\0', true); // Put an unescaped character
			waiting_escape_ = EWaitingEscape::NONE; // The escape sequence has ended
			break;
		}
		case u'a':
		case u'а':
		{
			next_(u'\a', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u'b':
		case u'б':
		{
			next_(u'\b', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u'f':
		case u'ф':
		{
			next_(u'\f', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u'n':
		case u'н':
		{
			next_(u'\n', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u'r':
		case u'р':
		{
			next_(u'\r', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u't':
		case u'т':
		{
			next_(u'\t', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u'v':
		case u'в':
		{
			next_(u'\v', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u'\n':
		{
			// Backslash in the end of the line continues the line
			next_(u' ', true);
			waiting_escape_ = EWaitingEscape::NONE;
			break;
		}
		case u'u':
		case u'ю':
		{
			// Unicode escape sequence
			waiting_escape_ = EWaitingEscape::U16;
			break;
		}
		case u'U':
		case u'Ю':
		{
			// Unicode escape sequence
			waiting_escape_ = EWaitingEscape::U32;
			break;
		}
		default:
		{
			// Unknown sequece: assume it means the character after the backslash: \* is *
			next_(c, true);
			waiting_escape_ = EWaitingEscape::NONE;
		}
		}
		break;
	}
	case EWaitingEscape::U16: // Either 4-digit or {}-sequence
	{
		if (c == u'{') // {}-sequence
		{
			waiting_escape_ = EWaitingEscape::U_BRACKETS;
		}
		else // 4-digit sequence
		{
			waiting_escape_ = EWaitingEscape::U16_STARTED;
			next_escape_char_(c); // The first digit
		}
		break;
	}
	case EWaitingEscape::U32: // Either 8-digit or {}-sequence
	{
		if (c == u'{') // {}-sequence
		{
			waiting_escape_ = EWaitingEscape::U_BRACKETS;
		}
		else // 8-digit sequence
		{
			waiting_escape_ = EWaitingEscape::U32_STARTED;
			next_escape_char_(c); // The first digit
		}
		break;
	}
	case EWaitingEscape::U16_STARTED:
	case EWaitingEscape::U32_STARTED:
	case EWaitingEscape::U_BRACKETS:
	{
		next_escape_char_(c);
		break;
	}
	}
}

TokenList Parser::extract_tokens()
{
	return std::move(tokens_);
}

void Parser::next_escape_char_(char16_t c)
{
	int chars_required;
	switch (waiting_escape_)
	{
	case EWaitingEscape::U16_STARTED:
		chars_required = 4;
		break;
	case EWaitingEscape::U32_STARTED:
		chars_required = 8;
		break;
	case EWaitingEscape::U_BRACKETS:
		// Any number of digits is possible (not quite though: apply_escape_() produces an error if there are more than 8 digits)
		chars_required = std::numeric_limits<int>::max();
		break;
	// Other values should be impossible
	// (but if they are, chars_required is zero, hence the error)
	}

	if (c >= u'0' && c <= u'9')
	{
		escape_code_ *= 0x10;
		escape_code_ += c - u'0';
	}
	else if (c >= u'A' && c <= u'F')
	{
		escape_code_ *= 0x10;
		escape_code_ += c - u'A' + 10;
	}
	else if (c >= u'a' && c <= u'f')
	{
		escape_code_ *= 0x10;
		escape_code_ += c - u'a' + 10;
	}
	else if (c == u'}' && waiting_escape_ == EWaitingEscape::U_BRACKETS) // A closing bracket is only possible in a {}-sequence
	{
		apply_escape_(); // The end of the sequence
		return;
	}
	else
	{
		// Error: invalid character
		std::shared_ptr<InvalidToken> token = std::make_shared<InvalidToken>();
		token->invalid_type = InvalidToken::EInvalidType::ESCAPE;
		tokens_.push_back(token);
		waiting_escape_ = EWaitingEscape::NONE; // The sequence has ended
		next(c); // Put the character as usual
		return;
	}
	
	if (++escape_chars_counter_ >= chars_required)
	{
		// The end of the sequence
		apply_escape_();
	}
}

void Parser::apply_escape_()
{
	if (escape_chars_counter_ > 8) // There can't be more than 8 digits
	{
		// Error
		std::shared_ptr<InvalidToken> token = std::make_shared<InvalidToken>();
		token->invalid_type = InvalidToken::EInvalidType::ESCAPE;
		tokens_.push_back(token);
	}
	else
	{
		// Convert a character code to UTF-16
		if (escape_code_ <= 0xFFFF)
		{
			char16_t c = escape_code_;
			next_(c, true);
		}
		else
		{
			escape_code_ -= 0b10000;
			char16_t c1 = 0xD800 + escape_code_ / 0x400;
			char16_t c2 = 0xDC00 + escape_code_ % 0x400;
			next_(c1, true);
			next_(c2, true);
		}
	}
	waiting_escape_ = EWaitingEscape::NONE;
}

// No incomplete tokens: create a new one
void Parser::new_token_(char16_t c)
{
	if (is_space(c))
	{
		// Ignore spaces
		waiting_.clear();
		waiting_status_ = EWaitingStatus::NONE;
	}
	else if (is_punctuator(c))
	{
		waiting_ = c;
		switch (c)
		{
		case u'\'':
			// The beginning of a character literal
			waiting_status_ = EWaitingStatus::CHAR_LITERAL;
			break;
		case u'"':
			// The beginning of a string literal
			waiting_status_ = EWaitingStatus::STRING_LITERAL;
			break;
		case u'.':
			// A punctuator or a float literal
			waiting_status_ = EWaitingStatus::DOT;
			break;
		default:
			waiting_status_ = EWaitingStatus::PUNCTUATOR;
			parse_punctuator_(false); // Try to parse
		}
	}
	else if (is_digit(c))
	{
		// The beginning of an integer or a float literal
		waiting_ = c;
		waiting_status_ =
			c == u'0' ? EWaitingStatus::ZERO_INT_LITERAL : EWaitingStatus::INT_LITERAL;
	}
	else
	{
		// The beginning of a word
		waiting_ = c;
		waiting_status_ = EWaitingStatus::WORD;
	}
}

