#include "Context.h"

void Context::execute(TokenList::const_iterator from, TokenList::const_iterator to)
{
	for (TokenList::const_iterator i = from; i != to; ++i)
	{
		(*i)->print();
	}
}
