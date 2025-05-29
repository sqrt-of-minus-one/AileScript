#include "Parser.h"

void Parser::parse_float_literal()
{
	if (waiting_.empty())
	{
		return;
	}

	std::shared_ptr<FloatLiteralToken> token = std::make_shared<FloatLiteralToken>();
	token->suffix = FloatLiteralToken::ESuffix::NONE;
	token->exp_sign = FloatLiteralToken::ESign::PLUS;

	// Indices of the point and exponent characters and the first characters of the suffix and postfix
	std::optional<int> point_pos, exp_pos, suffix_start, postfix_start;
	bool exp_has_sign = false;

	// Find point and exponent characters
	for (int i = 0; i < waiting_.length(); ++i)
	{
		if (is_digit(waiting_[i]))
		{
			continue;
		}

		switch (waiting_[i])
		{
		case u'\'':
		{
			// Ignore apostrophes
			break;
		}
		case u'.':
		{
			if (!point_pos && !exp_pos) // There can't be two points or a point after the exponent character
			{
				point_pos = i;
			}
			else
			{
				// We shouldn't be here: the interpreter should treat a point like this as a punctuator, not a part of the float literal
				suffix_start = i;
				i = waiting_.length(); // break from for
			}
			break;
		}
		case u'e':
		case u'E':
		case u'э':
		case u'Э':
		{
			if (!exp_pos) // If there hasn't been an exponent character
			{
				exp_pos = i;
				// The next character might be a sign
				switch (waiting_[i + 1]) // We don't get out of the array bounds: there is always a null character in the end
				{
				case u'+':
					exp_has_sign = true;
					token->exp_sign = FloatLiteralToken::ESign::PLUS;
					++i; // Skip the sign in the next iteration of for
					break;
				case u'-':
					exp_has_sign = true;
					token->exp_sign = FloatLiteralToken::ESign::MINUS;
					++i;
					break;
				}
			}
			else
			{
				// It's not an exponent character, it's a suffix/postfix (invalid though since neither of them can start with these characters)
				suffix_start = i;
				i = waiting_.length(); // break from for
			}
			break;
		}
		default:
		{
			suffix_start = i;
			i = waiting_.length(); // break from for
		}
		}
	}
	if (!suffix_start)
	{
		// No suffix/prefix
		suffix_start = waiting_.length();
	}
	
	postfix_start = suffix_start;
	switch (waiting_[*suffix_start])
	{
	case u'f':
	case u'F':
	case u'п':
	case u'П':
	{
		// float suffix
		token->suffix = FloatLiteralToken::ESuffix::FLOAT;
		postfix_start = *suffix_start + 1;
		break;
	}
	case u'l':
	case u'L':
	case u'д':
	case u'Д':
	{
		// ldouble suffix
		token->suffix = FloatLiteralToken::ESuffix::LDOUBLE;
		postfix_start = *suffix_start + 1;
		break;
	}
	}
	
	// Fill the token's fields
	if (exp_pos)
	{
		if (point_pos)
		{
			token->int_part = waiting_.substr(0, *point_pos);
			token->real_part = waiting_.substr(*point_pos + 1, *exp_pos - *point_pos - 1);
		}
		else
		{
			token->int_part = waiting_.substr(0, *exp_pos);
		}

		if (exp_has_sign)
		{
			token->exp_part = waiting_.substr(*exp_pos + 2, *suffix_start - *exp_pos - 2);
		}
		else
		{
			token->exp_part = waiting_.substr(*exp_pos + 1, *suffix_start - *exp_pos - 1);
		}
	}
	else
	{
		if (point_pos)
		{
			token->int_part = waiting_.substr(0, *point_pos);
			token->real_part = waiting_.substr(*point_pos + 1, *suffix_start - *point_pos - 1);
		}
		else
		{
			token->int_part = waiting_.substr(0, *suffix_start);
		}
	}
	token->postfix = waiting_.substr(*postfix_start);
	tokens_.push_back(token);

	waiting_.clear();
	waiting_status_ = EWaitingStatus::NONE;
}
