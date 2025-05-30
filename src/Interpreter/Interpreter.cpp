#include "Interpreter.h"

#include "../Parser/Parser.h"

void Interpreter::next(Token::Ptr token)
{
	tokens_.push_back(token);

	std::shared_ptr<PunctuatorToken> punctuator = std::dynamic_pointer_cast<PunctuatorToken>(token);
	if (punctuator)
	{
		switch (punctuator->punctuator)
		{
		case PunctuatorToken::EPunctuator::SEMICOLON:
		{
			if (to_be_closed_.empty())
			{
				global_context_.execute(tokens_.begin(), tokens_.end());
				tokens_.clear();
			}
			break;
		}
		case PunctuatorToken::EPunctuator::PAR_LEFT:
		case PunctuatorToken::EPunctuator::SQ_BRACKET_LEFT:
		case PunctuatorToken::EPunctuator::BRACKET_LEFT:
		{
			to_be_closed_.push(std::prev(tokens_.end()));
			break;
		}
		case PunctuatorToken::EPunctuator::PAR_RIGHT:
		{
			create_group_(GroupToken::EGroupType::PARENTHESIS, PunctuatorToken::EPunctuator::PAR_LEFT);
			break;
		}
		case PunctuatorToken::EPunctuator::SQ_BRACKET_RIGHT:
		{
			create_group_(GroupToken::EGroupType::SQUARE_BRACKET, PunctuatorToken::EPunctuator::SQ_BRACKET_LEFT);
			break;
		}
		case PunctuatorToken::EPunctuator::BRACKET_RIGHT:
		{
			create_group_(GroupToken::EGroupType::BRACKET, PunctuatorToken::EPunctuator::BRACKET_LEFT);
			break;
		}
		}
	}
}

void Interpreter::next(const TokenList& tokens)
{
	for (Token::Ptr token : tokens)
	{
		next(token);
	}
}

void Interpreter::extract_from_parser(Parser& parser)
{
	next(parser.extract_tokens());
}

Interpreter::EInputStatus Interpreter::get_input_status() const
{
	if (!to_be_closed_.empty())
	{
		std::shared_ptr<PunctuatorToken> not_closed = std::dynamic_pointer_cast<PunctuatorToken>(*to_be_closed_.top());
		switch (not_closed->punctuator)
		{
		case PunctuatorToken::EPunctuator::PAR_LEFT:
			return EInputStatus::PARENTHESIS;
		case PunctuatorToken::EPunctuator::SQ_BRACKET_LEFT:
			return EInputStatus::SQUARE_BRACKET;
		case PunctuatorToken::EPunctuator::BRACKET_LEFT:
			return EInputStatus::BRACKET;
		}
	}
	return EInputStatus::READY;
}

Interpreter& Interpreter::operator<<(Token::Ptr token)
{
	next(token);
	return *this;
}

Interpreter& Interpreter::operator<<(const TokenList& tokens)
{
	next(tokens);
	return *this;
}

Interpreter& Interpreter::operator<<(Parser& parser)
{
	extract_from_parser(parser);
	return *this;
}

void Interpreter::create_group_(GroupToken::EGroupType group_type, PunctuatorToken::EPunctuator required_left_punctuator)
{
	if (!to_be_closed_.empty())
	{
		TokenList::const_iterator left_it = to_be_closed_.top();
		std::shared_ptr<PunctuatorToken> left = std::dynamic_pointer_cast<PunctuatorToken>(*left_it);
		if (left && left->punctuator == required_left_punctuator)
		{
			std::shared_ptr<GroupToken> group = std::make_shared<GroupToken>();
			group->group_type = group_type;
			group->tokens.insert(group->tokens.end(), std::next(left_it), std::prev(tokens_.cend()));
			tokens_.erase(left_it, tokens_.end());
			tokens_.push_back(group);
			to_be_closed_.pop();
		}
	}
}
