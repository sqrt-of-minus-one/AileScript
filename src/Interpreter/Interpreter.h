#pragma once

#include "../Token.h"
#include "Context.h"

#include <stack>

class Parser;

class Interpreter
{
private:
	Context global_context_;

	TokenList tokens_;
	std::stack<TokenList::const_iterator> to_be_closed_;

public:
	enum class EInputStatus
	{
		READY,
		PARENTHESIS,
		SQUARE_BRACKET,
		BRACKET
	};

	void next(Token::Ptr token);
	void next(const TokenList& tokens);
	void extract_from_parser(Parser& parser);

	EInputStatus get_input_status() const;
	
	Interpreter& operator<<(Token::Ptr token);
	Interpreter& operator<<(const TokenList& tokens);
	Interpreter& operator<<(Parser& parser);
};
