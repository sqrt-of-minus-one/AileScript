#include "Parser.h"

// escaped is true if the current character has been put using an escape sequence
void Parser::next_(char16_t c, bool escaped)
{
	switch (waiting_status_)
	{
	case EWaitingStatus::NONE:
	{
		// No incomplete tokens: create a new one
		new_token_(c);
		break;
	}
	case EWaitingStatus::WORD:
	{
		if (is_space(c))
		{
			// The end of the word
			parse_word_();
		}
		else if (is_punctuator(c))
		{
			switch (c)
			{
			case u'\'':
			{
				// The word turned out to be a prefix of a character literal
				waiting_status_ = EWaitingStatus::CHAR_LITERAL;
				waiting_.push_back(c);
				break;
			}
			case u'"':
			{
				// The word turned out to be a prefix of a string literal
				waiting_status_ = EWaitingStatus::STRING_LITERAL;
				waiting_.push_back(c);
				break;
			}
			default:
			{
				// The end of the word, a punctuator is a part of a new token
				parse_word_();
				new_token_(c);
			}
			}
		}
		else
		{
			// The word continues
			waiting_.push_back(c);
		}
		break;
	}
	case EWaitingStatus::DOT:
	{
		if (is_digit(c))
		{
			// The dot turned out to be a part of a float literal
			waiting_.push_back(c);
			waiting_status_ = EWaitingStatus::FLOAT_LITERAL;
			break;
		}

		// If not, treat the dot as a punctuator
		waiting_status_ = EWaitingStatus::PUNCTUATOR;
		[[fallthrough]];
	}
	case EWaitingStatus::PUNCTUATOR:
	{
		if (is_punctuator(c))
		{
			// The punctuator continues
			waiting_.push_back(c);
			parse_punctuator_(false);
		}
		else
		{
			// The end of the punctuator, the character is a part of a new token
			parse_punctuator_(true);
			new_token_(c);
		}
		break;
	}
	case EWaitingStatus::INT_LITERAL:
	{
		if (is_space(c))
		{
			// The end of the literal
			parse_int_literal();
		}
		else if (is_punctuator(c))
		{
			switch (c)
			{
			case u'\'':
			{
				// Ignore apostrophes
				waiting_.push_back(c);
				break;
			}
			case u'.':
			{
				// The point means that the literal is float
				waiting_.push_back(c);
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL;
				break;
			}
			default:
			{
				// The end of the literal
				parse_int_literal();
				new_token_(c);
			}
			}
		}
		else if (is_digit(c))
		{
			// Another digit of the literal
			waiting_.push_back(c);
		}
		else
		{
			waiting_.push_back(c);
			switch (c)
			{
			case u'e':
			case u'E':
			case u'э':
			case u'Э':
				// The literal turned out to be a float in exponential form
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL_EXP_SIGN;
				break;
			default:
				// The suffix/postfix has started
				waiting_status_ = EWaitingStatus::INT_LITERAL_SUFFIX;
			}
		}
		break;
	}
	case EWaitingStatus::ZERO_INT_LITERAL:
	{
		if (is_space(c))
		{
			// The end of the literal
			parse_int_literal();
		}
		else if (is_punctuator(c))
		{
			switch (c)
			{
			case u'\'':
			{
				// An apostrophe cannot appear inside a prefix
				waiting_.push_back(c);
				waiting_status_ = EWaitingStatus::ZERO_OCT_INT_LITERAL;
				break;
			}
			case u'.':
			{
				// The point means that the literal is float
				waiting_.push_back(c);
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL;
				break;
			}
			default:
			{
				// The end of the literal
				parse_int_literal();
				new_token_(c);
			}
			}
		}
		else if (is_digit(c))
		{
			// Another digit of the literal
			// The digit must be octal, but if not, the error will appear later
			waiting_.push_back(c);
		}
		else
		{
			waiting_.push_back(c);
			switch (c)
			{
			case u'x':
			case u'X':
			case u'h':
			case u'H':
			case u'ш':
			case u'Ш':
				// The hexadecimal prefix
				waiting_status_ = EWaitingStatus::HEX_INT_LITERAL;
				break;
			case u'o':
			case u'O':
			case u'о':
			case u'О':
				// The octal prefix
				waiting_status_ = EWaitingStatus::OCT_INT_LITERAL;
				break;
			case u'b':
			case u'B':
			case u'д':
			case u'Д':
			case u'б':
			case u'Б':
				// The binary prefix
				waiting_status_ = EWaitingStatus::BIN_INT_LITERAL;
				break;
			case u'e':
			case u'E':
			case u'э':
			case u'Э':
				// The literal turned out to be a float in exponential form
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL_EXP_SIGN;
				break;
			default:
				// The suffix/postfix has started
				waiting_status_ = EWaitingStatus::INT_LITERAL_SUFFIX;
			}
		}
		break;
	}
	case EWaitingStatus::HEX_INT_LITERAL:
	case EWaitingStatus::OCT_INT_LITERAL:
	case EWaitingStatus::BIN_INT_LITERAL:
	{
		if (is_hex_digit(c) || c == u'\'')
		{
			// Another digit of the literal
			waiting_.push_back(c);
		}
		else if (is_space(c) || is_punctuator(c))
		{
			// The end of the literal
			parse_int_literal();
			new_token_(c);
		}
		else
		{
			// The suffix/postfix has started
			waiting_.push_back(c);
			waiting_status_ = EWaitingStatus::INT_LITERAL_SUFFIX;
		}
		break;
	}
	case EWaitingStatus::ZERO_OCT_INT_LITERAL:
	{
		if (is_space(c))
		{
			// The end of the literal
			parse_int_literal();
		}
		else if (is_punctuator(c))
		{
			switch (c)
			{
			case u'\'':
			{
				// Ignore apostrophes
				waiting_.push_back(c);
				break;
			}
			case u'.':
			{
				// The point means that the literal is float
				waiting_.push_back(c);
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL;
				break;
			}
			default:
			{
				// The end of the literal
				parse_int_literal();
				new_token_(c);
			}
			}
		}
		else if (is_digit(c))
		{
			// Another digit of the literal
			// The digit must be octal, but if not, the error will appear later
			waiting_.push_back(c);
		}
		else
		{
			waiting_.push_back(c);
			switch (c)
			{
			case u'e':
			case u'E':
			case u'э':
			case u'Э':
				// The literal turned out to be a float in exponential form
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL_EXP_SIGN;
				break;
			default:
				// The suffix/postfix has started
				waiting_status_ = EWaitingStatus::INT_LITERAL_SUFFIX;
			}
		}
		break;
	}
	case EWaitingStatus::INT_LITERAL_SUFFIX:
	{
		if (is_space(c) || is_punctuator(c))
		{
			// The end of the literal
			parse_int_literal();
			new_token_(c);
		}
		else
		{
			waiting_.push_back(c);
		}
		break;
	}
	case EWaitingStatus::FLOAT_LITERAL:
	{
		if (is_digit(c) || c == u'\'')
		{
			waiting_.push_back(c);
		}
		else if (is_space(c) || is_punctuator(c))
		{
			// The end of the literal
			parse_float_literal();
			new_token_(c);
		}
		else
		{
			waiting_.push_back(c);
			switch (c)
			{
			case u'e':
			case u'E':
			case u'э':
			case u'Э':
				// The exponential part starts
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL_EXP_SIGN;
				break;
			default:
				// The suffix/postfix has started
				waiting_status_ = EWaitingStatus::FLOAT_LITERAL_SUFFIX;
			}
		}
		break;
	}
	case EWaitingStatus::FLOAT_LITERAL_EXP_SIGN:
	{
		if (c == u'+' || c == u'-')
		{
			// The sign here should not be treated as a punctuator
			waiting_.push_back(c);
			break;
		}
		waiting_status_ = EWaitingStatus::FLOAT_LITERAL_EXP;
		[[fallthrough]];
	}
	case EWaitingStatus::FLOAT_LITERAL_EXP:
	{
		if (is_digit(c) || c == u'\'')
		{
			waiting_.push_back(c);
		}
		else if (is_space(c) || is_punctuator(c))
		{
			// The end of the literal
			parse_float_literal();
			new_token_(c);
		}
		else
		{
			// The suffix/postfix has started
			waiting_.push_back(c);
			waiting_status_ = EWaitingStatus::FLOAT_LITERAL_SUFFIX;
		}
		break;
	}
	case EWaitingStatus::FLOAT_LITERAL_SUFFIX:
	{
		if (is_space(c) || is_punctuator(c))
		{
			// The end of the literal
			parse_float_literal();
			new_token_(c);
		}
		else
		{
			waiting_.push_back(c);
		}
		break;
	}
	case EWaitingStatus::CHAR_LITERAL:
	{
		waiting_.push_back(c);
		if (c == u'\'' && !escaped)
		{
			// The end of the character value
			waiting_status_ = EWaitingStatus::CHAR_LITERAL_SUFFIX;
		}
		break;
	}
	case EWaitingStatus::CHAR_LITERAL_SUFFIX:
	{
		if (is_space(c) || is_punctuator(c))
		{
			// The end of the literal
			parse_char_literal();
			new_token_(c);
		}
		else
		{
			waiting_.push_back(c);
		}
		break;
	}
	case EWaitingStatus::STRING_LITERAL:
	{
		waiting_.push_back(c);
		if (c == u'"' && !escaped)
		{
			// The end of the string value
			waiting_status_ = EWaitingStatus::STRING_LITERAL_SUFFIX;
		}
		break;
	}
	case EWaitingStatus::STRING_LITERAL_SUFFIX:
	{
		if (is_space(c) || is_punctuator(c))
		{
			// The end of the literal
			parse_string_literal();
			new_token_(c);
		}
		else
		{
			waiting_.push_back(c);
		}
		break;
	}
	case EWaitingStatus::SINGLE_LINE_COMMENT:
	{
		switch (c)
		{
		case u'\\':
			waiting_status_ = EWaitingStatus::SINGLE_LINE_COMMENT_BACKSLASH;
			break;
		case u'\n':
			// The end of the comment
			waiting_status_ = EWaitingStatus::NONE;
			break;
		}
		break;
	}
	case EWaitingStatus::SINGLE_LINE_COMMENT_BACKSLASH:
	{
		if (c != u'\\')
		{
			// The comment continues event if c is \n
			waiting_status_ = EWaitingStatus::SINGLE_LINE_COMMENT;
		}
		break;
	}
	case EWaitingStatus::MULTI_LINE_COMMENT:
	{
		if (c == u'*')
		{
			// If the next character is /, it will be the end of the comment
			waiting_status_ = EWaitingStatus::MULTI_LINE_COMMENT_ENDING;
		}
		break;
	}
	case EWaitingStatus::MULTI_LINE_COMMENT_ENDING:
	{
		if (c == u'/')
		{
			// The end of the comment
			waiting_status_ = EWaitingStatus::NONE;
		}
		break;
	}
	}
}
