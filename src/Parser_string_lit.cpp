#include "Parser.h"

void Parser::parse_string_literal()
{
	if (waiting_.empty())
	{
		return;
	}

	std::shared_ptr<StringLiteralToken> token = std::make_shared<StringLiteralToken>();

	// Positions of opening and closing quotes
	int left_quote = 0, right_quote = 0;

	// Prefix
	switch (waiting_[0])
	{
	case u'"':
	{
		// No prefix
		left_quote = 0;
		break;
	}
	case u'u':
	case u'U':
	case u'ю':
	case u'Ю':
	{
		if (waiting_[1] == u'"')
		{
			token->prefix = (waiting_[0] == u'u' || waiting_[0] == u'ю') ? StringLiteralToken::EPrefix::U8 : StringLiteralToken::EPrefix::U32;
			left_quote = 1;
			break;
		}
		else if (waiting_[1] == u'8' && waiting_[2] == u'"')
		{
			token->prefix = StringLiteralToken::EPrefix::U8;
			left_quote = 2;
			break;
		}
		else if (waiting_[1] == u'1' && waiting_[2] == u'6' && waiting_[3] == u'"')
		{
			token->prefix = StringLiteralToken::EPrefix::U16;
			left_quote = 2;
			break;
		}
		else if (waiting_[1] == u'3' && waiting_[2] == u'2' && waiting_[3] == u'"')
		{
			token->prefix = StringLiteralToken::EPrefix::U32;
			left_quote = 2;
			break;
		}
		else [[fallthrough]];
	}
	default:
	{
		// Unknown prefix
		token->prefix = StringLiteralToken::EPrefix::INVALID;
		left_quote = waiting_.find_first_of(u'"');
	}
	}
	right_quote = waiting_.find_last_of(u'"');

	token->value = waiting_.substr(left_quote + 1, right_quote - left_quote - 1);
	token->postfix = waiting_.substr(right_quote + 1);
	tokens_.push_back(token);

	waiting_.clear();
	waiting_status_ = EWaitingStatus::NONE;
}
