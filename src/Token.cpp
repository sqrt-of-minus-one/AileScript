#include "Token.h"

#include "encoding.h"

#include <iomanip>

void InvalidToken::print() const
{
	switch (invalid_type)
	{
	case EInvalidType::ESCAPE:
		std::cout << "Unknown escape sequence" << std::endl;
		break;
	default:
		std::cout << "Unknown invalid token" << std::endl;
	}
}

void GroupToken::print() const
{
	std::cout << std::left << std::setw(20) << "Group: ";
	switch (group_type)
	{
	case EGroupType::PARENTHESIS:
		std::cout << '(' << std::endl;
		break;
	case EGroupType::SQUARE_BRACKET:
		std::cout << '[' << std::endl;
		break;
	case EGroupType::BRACKET:
		std::cout << '{' << std::endl;
		break;
	}

	for (Ptr token : tokens)
	{
		token->print();
	}

	std::cout << std::left << std::setw(20) << "Group: ";
	switch (group_type)
	{
	case EGroupType::PARENTHESIS:
		std::cout << ')' << std::endl;
		break;
	case EGroupType::SQUARE_BRACKET:
		std::cout << ']' << std::endl;
		break;
	case EGroupType::BRACKET:
		std::cout << '}' << std::endl;
		break;
	}
}

void IdToken::print() const
{
	std::cout << std::left << std::setw(20) << "Identifier: " << name << std::endl;
}

void KeywordToken::print() const
{
	std::cout << std::left << std::setw(20) << "Keyword: ";
	switch (keyword)
	{
	case EKeyword::BYTE:
		std::cout << "byte" << std::endl;
		break;
	case EKeyword::SHORT:
		std::cout << "short" << std::endl;
		break;
	case EKeyword::INT:
		std::cout << "int" << std::endl;
		break;
	case EKeyword::LONG:
		std::cout << "long" << std::endl;
		break;
	case EKeyword::UBYTE:
		std::cout << "ubyte" << std::endl;
		break;
	case EKeyword::USHORT:
		std::cout << "ushort" << std::endl;
		break;
	case EKeyword::UINT:
		std::cout << "uint" << std::endl;
		break;
	case EKeyword::ULONG:
		std::cout << "ulong" << std::endl;
		break;
	case EKeyword::BOOL:
		std::cout << "bool" << std::endl;
		break;
	case EKeyword::FLOAT:
		std::cout << "float" << std::endl;
		break;
	case EKeyword::DOUBLE:
		std::cout << "double" << std::endl;
		break;
	case EKeyword::LDOUBLE:
		std::cout << "ldouble" << std::endl;
		break;
	case EKeyword::CHAR:
		std::cout << "char" << std::endl;
		break;
	case EKeyword::CHAR8:
		std::cout << "char8" << std::endl;
		break;
	case EKeyword::CHAR16:
		std::cout << "char16" << std::endl;
		break;
	case EKeyword::CHAR32:
		std::cout << "char32" << std::endl;
		break;
	case EKeyword::FNC:
		std::cout << "fnc" << std::endl;
		break;
	case EKeyword::AUTO:
		std::cout << "auto" << std::endl;
		break;
	case EKeyword::IF:
		std::cout << "if" << std::endl;
		break;
	case EKeyword::ELSE:
		std::cout << "else" << std::endl;
		break;
	case EKeyword::FOR:
		std::cout << "for" << std::endl;
		break;
	case EKeyword::WHILE:
		std::cout << "while" << std::endl;
		break;
	case EKeyword::DO:
		std::cout << "do" << std::endl;
		break;
	}
}

void PunctuatorToken::print() const
{
	std::cout << std::left << std::setw(20) << "Punctuator: ";
	switch (punctuator)
	{
		
	case EPunctuator::NOT:
		std::cout << "!" << std::endl;
		break;
	case EPunctuator::NOT_EQ:
		std::cout << "!=" << std::endl;
		break;
	case EPunctuator::QUOTE:
		std::cout << "\"" << std::endl;
		break;
	case EPunctuator::CONST:
		std::cout << "#" << std::endl;
		break;
	case EPunctuator::CONSTEXPR:
		std::cout << "##" << std::endl;
		break;
	case EPunctuator::DOLLAR:
		std::cout << "$" << std::endl;
		break;
	case EPunctuator::MOD:
		std::cout << "%" << std::endl;
		break;
	case EPunctuator::MOD_AS:
		std::cout << "%=" << std::endl;
		break;
	case EPunctuator::BIT_AND:
		std::cout << "&" << std::endl;
		break;
	case EPunctuator::AND:
		std::cout << "&&" << std::endl;
		break;
	case EPunctuator::AND_AS:
		std::cout << "&&=" << std::endl;
		break;
	case EPunctuator::BIT_AND_AS:
		std::cout << "&=" << std::endl;
		break;
	case EPunctuator::SINGLE_QUOTE:
		std::cout << "'" << std::endl;
		break;
	case EPunctuator::PAR_LEFT:
		std::cout << "(" << std::endl;
		break;
	case EPunctuator::PAR_RIGHT:
		std::cout << ")" << std::endl;
		break;
	case EPunctuator::MUL:
		std::cout << "*" << std::endl;
		break;
	case EPunctuator::COMMENT_END:
		std::cout << "*/" << std::endl;
		break;
	case EPunctuator::MUL_AS:
		std::cout << "*=" << std::endl;
		break;
	case EPunctuator::PLUS:
		std::cout << "+" << std::endl;
		break;
	case EPunctuator::INC:
		std::cout << "++" << std::endl;
		break;
	case EPunctuator::PLUS_AS:
		std::cout << "+=" << std::endl;
		break;
	case EPunctuator::COMMA:
		std::cout << "," << std::endl;
		break;
	case EPunctuator::MINUS:
		std::cout << "-" << std::endl;
		break;
	case EPunctuator::DEC:
		std::cout << "--" << std::endl;
		break;
	case EPunctuator::MINUS_AS:
		std::cout << "-=" << std::endl;
		break;
	case EPunctuator::PTR_MEMBER:
		std::cout << "->" << std::endl;
		break;
	case EPunctuator::DOT:
		std::cout << "." << std::endl;
		break;
	case EPunctuator::RANGE:
		std::cout << ".." << std::endl;
		break;
	case EPunctuator::ELLIPSIS:
		std::cout << "..." << std::endl;
		break;
	case EPunctuator::DIV:
		std::cout << "/" << std::endl;
		break;
	case EPunctuator::COMMENT_START:
		std::cout << "/*" << std::endl;
		break;
	case EPunctuator::COMMENT:
		std::cout << "//" << std::endl;
		break;
	case EPunctuator::INT_DIV:
		std::cout << "/:" << std::endl;
		break;
	case EPunctuator::INT_DIV_AS:
		std::cout << "/:=" << std::endl;
		break;
	case EPunctuator::DIV_AS:
		std::cout << "/=" << std::endl;
		break;
	case EPunctuator::COLON:
		std::cout << ":" << std::endl;
		break;
	case EPunctuator::SCOPE:
		std::cout << "::" << std::endl;
		break;
	case EPunctuator::SEMICOLON:
		std::cout << ";" << std::endl;
		break;
	case EPunctuator::LESS:
		std::cout << "<" << std::endl;
		break;
	case EPunctuator::SHIFT_LEFT:
		std::cout << "<<" << std::endl;
		break;
	case EPunctuator::SHIFT_LEFT_AS:
		std::cout << "<<=" << std::endl;
		break;
	case EPunctuator::LESS_EQ:
		std::cout << "<=" << std::endl;
		break;
	case EPunctuator::SPACESHIP:
		std::cout << "<=>" << std::endl;
		break;
	case EPunctuator::STATIC_CAST:
		std::cout << "static_cast" << std::endl;
		break;
	case EPunctuator::DYNAMIC_CAST:
		std::cout << "dynamic_cast" << std::endl;
		break;
	case EPunctuator::CONST_CAST:
		std::cout << "const_cast" << std::endl;
		break;
	case EPunctuator::REINTERPRET_CAST:
		std::cout << "reinterpret_cast" << std::endl;
		break;
	case EPunctuator::ASSIGN:
		std::cout << "=" << std::endl;
		break;
	case EPunctuator::EQUAL:
		std::cout << "==" << std::endl;
		break;
	case EPunctuator::RETURN:
		std::cout << "=>" << std::endl;
		break;
	case EPunctuator::GREATER:
		std::cout << ">" << std::endl;
		break;
	case EPunctuator::GREATER_EQ:
		std::cout << ">=" << std::endl;
		break;
	case EPunctuator::SHIFT_RIGHT:
		std::cout << ">>" << std::endl;
		break;
	case EPunctuator::SHIFT_RIGHT_AS:
		std::cout << ">>=" << std::endl;
		break;
	case EPunctuator::QUESTION:
		std::cout << "?" << std::endl;
		break;
	case EPunctuator::AT:
		std::cout << "@" << std::endl;
		break;
	case EPunctuator::SQ_BRACKET_LEFT:
		std::cout << "[" << std::endl;
		break;
	case EPunctuator::BACKSLASH:
		std::cout << "\\" << std::endl;
		break;
	case EPunctuator::SQ_BRACKET_RIGHT:
		std::cout << "]" << std::endl;
		break;
	case EPunctuator::XOR:
		std::cout << "^" << std::endl;
		break;
	case EPunctuator::XOR_AS:
		std::cout << "^=" << std::endl;
		break;
	case EPunctuator::POW:
		std::cout << "^^" << std::endl;
		break;
	case EPunctuator::POW_AS:
		std::cout << "^^=" << std::endl;
		break;
	case EPunctuator::APOSTROPHE:
		std::cout << "`" << std::endl;
		break;
	case EPunctuator::BRACKET_LEFT:
		std::cout << "{" << std::endl;
		break;
	case EPunctuator::BIT_OR:
		std::cout << "|" << std::endl;
		break;
	case EPunctuator::BIT_OR_AS:
		std::cout << "|=" << std::endl;
		break;
	case EPunctuator::OR:
		std::cout << "||" << std::endl;
		break;
	case EPunctuator::OR_AS:
		std::cout << "||=" << std::endl;
		break;
	case EPunctuator::BRACKET_RIGHT:
		std::cout << "}" << std::endl;
		break;
	case EPunctuator::COMPL:
		std::cout << "~" << std::endl;
		break;
	case EPunctuator::COMPL_AS:
		std::cout << "~=" << std::endl;
		break;
	}
}

void IntLiteralToken::print() const
{
	std::cout << std::left << std::setw(20) << "Integer literal: " << value << " (";

	switch (prefix)
	{
	case EPrefix::NONE:
		std::cout << "decimal, ";
		break;
	case EPrefix::HEX:
		std::cout << "hexadecimal, ";
		break;
	case EPrefix::OCT:
		std::cout << "octal, ";
		break;
	case EPrefix::BIN:
		std::cout << "binary, ";
		break;
	}

	switch (suffix)
	{
	case ESuffix::NONE:
		std::cout << "int)";
		break;
	case ESuffix::BYTE:
		std::cout << "byte)";
		break;
	case ESuffix::SHORT:
		std::cout << "short)";
		break;
	case ESuffix::LONG:
		std::cout << "long)";
		break;
	case ESuffix::UBYTE:
		std::cout << "ubyte)";
		break;
	case ESuffix::USHORT:
		std::cout << "ushort)";
		break;
	case ESuffix::UINT:
		std::cout << "uint)";
		break;
	case ESuffix::ULONG:
		std::cout << "ulong)";
		break;
	}

	if (!postfix.empty())
	{
		std::cout << ", postfix: " << postfix;
	}
	std::cout << std::endl;
}

void FloatLiteralToken::print() const
{
	std::cout << std::left << std::setw(20) << "Float literal: " << int_part << '.' << real_part;
	if (!exp_part.empty())
	{
		std::cout << 'e' << (exp_sign == ESign::PLUS ? '+' : '-') << exp_part;
	}

	switch (suffix)
	{
	case ESuffix::NONE:
		std::cout << " (double)";
		break;
	case ESuffix::FLOAT:
		std::cout << " (float)";
		break;
	case ESuffix::LDOUBLE:
		std::cout << " (ldouble)";
		break;
	}
	
	if (!postfix.empty())
	{
		std::cout << ", postfix: " << postfix;
	}
	std::cout << std::endl;
}

void CharLiteralToken::print() const
{
	std::cout << std::left << std::setw(20) << "Char literal: " << '\'' << value << "' (";

	switch (prefix)
	{
	case EPrefix::NONE:
		std::cout << "no prefix)";
		break;
	case EPrefix::INVALID:
		std::cout << "invalid prefix)";
		break;
	case EPrefix::U8:
		std::cout << "u8)";
		break;
	case EPrefix::U16:
		std::cout << "u16)";
		break;
	case EPrefix::U32:
		std::cout << "u32)";
		break;
	}
	
	if (!postfix.empty())
	{
		std::cout << ", postfix: " << postfix;
	}
	std::cout << std::endl;
}

void StringLiteralToken::print() const
{
	std::cout << std::left << std::setw(20) << "String literal: " << '"' << value << "\" (";

	switch (prefix)
	{
	case EPrefix::NONE:
		std::cout << "no prefix)";
		break;
	case EPrefix::INVALID:
		std::cout << "invalid prefix)";
		break;
	case EPrefix::U8:
		std::cout << "u8)";
		break;
	case EPrefix::U16:
		std::cout << "u16)";
		break;
	case EPrefix::U32:
		std::cout << "u32)";
		break;
	}
	
	if (!postfix.empty())
	{
		std::cout << ", postfix: " << postfix;
	}
	std::cout << std::endl;
}
