#pragma once

#include "../Token.h"

class Context
{
public:
	void execute(TokenList::const_iterator from, TokenList::const_iterator to);
};
