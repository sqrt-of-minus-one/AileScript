#include "Parser.h"

#include <map>

// The map of all AileScript keywords
const std::map<std::u16string, KeywordToken::EKeyword> KEYWORDS = {
	{ u"byte",		KeywordToken::EKeyword::BYTE },
	{ u"short",		KeywordToken::EKeyword::SHORT },
	{ u"int",		KeywordToken::EKeyword::INT },
	{ u"long",		KeywordToken::EKeyword::LONG },
	{ u"ubyte",		KeywordToken::EKeyword::UBYTE },
	{ u"ushort",	KeywordToken::EKeyword::USHORT },
	{ u"uint",		KeywordToken::EKeyword::UINT },
	{ u"ulong",		KeywordToken::EKeyword::ULONG },
	{ u"bool",		KeywordToken::EKeyword::BOOL },
	{ u"float",		KeywordToken::EKeyword::FLOAT },
	{ u"double",	KeywordToken::EKeyword::INT },
	{ u"ldouble",	KeywordToken::EKeyword::LDOUBLE },
	{ u"char",		KeywordToken::EKeyword::CHAR },
	{ u"char8",		KeywordToken::EKeyword::CHAR8 },
	{ u"char16",	KeywordToken::EKeyword::CHAR16 },
	{ u"char32",	KeywordToken::EKeyword::CHAR32 },
	{ u"fnc",		KeywordToken::EKeyword::FNC },

	{ u"if",		KeywordToken::EKeyword::IF },
	{ u"else",		KeywordToken::EKeyword::ELSE },
	{ u"for",		KeywordToken::EKeyword::FOR },
	{ u"while",		KeywordToken::EKeyword::WHILE },
	{ u"do",		KeywordToken::EKeyword::DO },

	{ u"байт",		KeywordToken::EKeyword::BYTE },
	{ u"кор",		KeywordToken::EKeyword::SHORT },
	{ u"цел",		KeywordToken::EKeyword::INT },
	{ u"длин",		KeywordToken::EKeyword::LONG },
	{ u"ббайт",		KeywordToken::EKeyword::UBYTE },
	{ u"бкор",		KeywordToken::EKeyword::USHORT },
	{ u"бцел",		KeywordToken::EKeyword::UINT },
	{ u"бдлин",		KeywordToken::EKeyword::ULONG },
	{ u"бул",		KeywordToken::EKeyword::BOOL },
	{ u"плав",		KeywordToken::EKeyword::FLOAT },
	{ u"вещ",		KeywordToken::EKeyword::INT },
	{ u"двещ",		KeywordToken::EKeyword::LDOUBLE },
	{ u"симв",		KeywordToken::EKeyword::CHAR },
	{ u"симв8",		KeywordToken::EKeyword::CHAR8 },
	{ u"симв16",	KeywordToken::EKeyword::CHAR16 },
	{ u"симв32",	KeywordToken::EKeyword::CHAR32 },
	{ u"фнц",		KeywordToken::EKeyword::FNC },

	{ u"если",		KeywordToken::EKeyword::IF },
	{ u"иначе",		KeywordToken::EKeyword::ELSE },
	{ u"для",		KeywordToken::EKeyword::FOR },
	{ u"пока",		KeywordToken::EKeyword::WHILE },
	{ u"делать",	KeywordToken::EKeyword::DO },
};

void Parser::parse_word_()
{
	auto keyword = KEYWORDS.find(waiting_);
	if (keyword == KEYWORDS.end())
	{
		// Not a keyword, save it as an identifier
		std::shared_ptr<IdToken> token = std::make_shared<IdToken>();
		token->name = waiting_;
		tokens_.push_back(token);
	}
	else
	{
		// A word is a keyword
		std::shared_ptr<KeywordToken> token = std::make_shared<KeywordToken>();
		token->keyword = keyword->second;
		tokens_.push_back(token);
	}
	waiting_.clear();
	waiting_status_ = EWaitingStatus::NONE;
}
