#include "Parser.h"

// is_end is true, if the next character is not a punctuator
// E.g. if waiting_ == "<=" and is_end == false, we don't create a <= token, because the next character can be >. In this case we'll have to create a <=> token.
//	But if is_end == true, we know the next character is not > and create a <= token
void Parser::parse_punctuator_(bool is_end)
{
	// Some defines allow the monstrous code below not to be even more monstrous

	// Creates a new token of the given type and pushes it into tokens_, erases the first nchar characters from waiting_
	#define NEW_PUNCTUATOR(type, nchar) { \
		std::shared_ptr<PunctuatorToken> ptr = std::make_shared<PunctuatorToken>(); \
		ptr->punctuator = PunctuatorToken::EPunctuator::type; \
		tokens_.push_back(ptr); \
		waiting_.erase(0, nchar); }

	// Try to parse until the string is empty
	while (!waiting_.empty())
	{
		// Sequentially check every character and determine the punctuator type
		switch (waiting_[0])
		{
		case u'!':
		{
			switch (waiting_[1])
			{
			case u'=':
				NEW_PUNCTUATOR(NOT_EQ, 2); // !=
				break;
			case u'\0':
				if (!is_end) return; // If the next character is unknown, leave the current string as is and return from the function
				[[fallthrough]]; // If the next character will not be a punctuator, create a token
			default:
				NEW_PUNCTUATOR(NOT, 1); // !
			}
			break;
		}
		case u'"':
		{
			// We shouldn't be here: " should create a string literal, not a punctuator
			NEW_PUNCTUATOR(QUOTE, 1); // "
			break;
		}
		case u'#':
		{
			switch (waiting_[1])
			{
			case u'#':
				NEW_PUNCTUATOR(CONSTEXPR, 2); // ##
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(CONST, 1); // #
			}
			break;
		}
		case u'$':
		{
			NEW_PUNCTUATOR(DOLLAR, 1); // $
			break;
		}
		case u'%':
		{
			switch (waiting_[1])
			{
			case u'=':
				NEW_PUNCTUATOR(MOD_AS, 2); // %=
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(MOD, 1); // %
			}
			break;
		}
		case u'&':
		{
			switch (waiting_[1])
			{
			case u'&':
				switch (waiting_[2])
				{
				case u'=':
					NEW_PUNCTUATOR(AND_AS, 3); // &&=
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(AND, 2); // &&
				}
				break;
			case u'=':
				NEW_PUNCTUATOR(BIT_AND_AS, 2); // &=
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(BIT_AND, 1); // &
			}
			break;
		}
		case u'\'':
		{
			// We shouldn't be here: ' should create a character literal, not a punctuator
			NEW_PUNCTUATOR(SINGLE_QUOTE, 1); // '
			break;
		}
		case u'(':
		{
			NEW_PUNCTUATOR(PAR_LEFT, 1); // (
			break;
		}
		case u')':
		{
			NEW_PUNCTUATOR(PAR_RIGHT, 1); // )
			break;
		}
		case u'*':
		{
			switch (waiting_[1])
			{
			case u'/':
				// The interpreter ignores everything after /*
				// If we're here, it means that */ appears without /*, which eventually will lead to an error
				NEW_PUNCTUATOR(COMMENT_END, 2); // */
				break;
			case u'=':
				NEW_PUNCTUATOR(MUL_AS, 2); // *=
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(MUL, 1); // *
			}
			break;
		}
		case u'+':
		{
			switch (waiting_[1])
			{
			case u'+':
				NEW_PUNCTUATOR(INC, 2); // ++
				break;
			case u'=':
				NEW_PUNCTUATOR(PLUS_AS, 2); // +=
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(PLUS, 1); // +
			}
			break;
		}
		case u',':
		{
			NEW_PUNCTUATOR(COMMA, 1); // ,
			break;
		}
		case u'-':
		{
			switch (waiting_[1])
			{
			case u'-':
				NEW_PUNCTUATOR(DEC, 2); // --
				break;
			case u'=':
				NEW_PUNCTUATOR(MINUS_AS, 2); // -=
				break;
			case u'>':
				NEW_PUNCTUATOR(PTR_MEMBER, 2); // ->
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(MINUS, 1); // -
			}
			break;
		}
		case u'.':
		{
			switch (waiting_[1])
			{
			case u'.':
				switch (waiting_[2])
				{
				case u'.':
					NEW_PUNCTUATOR(ELLIPSIS, 3); // ...
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(RANGE, 2); // ..
				}
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(DOT, 1); // .
			}
			break;
		}
		case u'/':
		{
			switch (waiting_[1])
			{
			case u'*':
				// We don't create a token, we just skip everything till */
				waiting_status_ = EWaitingStatus::MULTI_LINE_COMMENT; // /*
				waiting_.clear();
				return;
			case u'/':
				// We don't create a token, we just skip everything till the end of the line
				waiting_status_ = EWaitingStatus::SINGLE_LINE_COMMENT; // //
				waiting_.clear();
				return;
			case u':':
				switch (waiting_[2])
				{
				case u'=':
					NEW_PUNCTUATOR(INT_DIV_AS, 3); // /:=
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(INT_DIV, 2); // /:
				}
				break;
			case u'=':
				NEW_PUNCTUATOR(DIV_AS, 2); // /=
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(DIV, 1); // /
			}
			break;
		}
		case u':':
		{
			switch (waiting_[1])
			{
			case u':':
				NEW_PUNCTUATOR(SCOPE, 2); // ::
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(COLON, 1); // :
			}
			break;
		}
		case u';':
		{
			NEW_PUNCTUATOR(SEMICOLON, 1); // ;
			break;
		}
		case u'<':
		{
			switch (waiting_[1])
			{
			case u'<':
				switch (waiting_[2])
				{
				case u'=':
					NEW_PUNCTUATOR(SHIFT_LEFT_AS, 3); // <<=
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(SHIFT_LEFT, 2); // <<
				}
				break;
			case u'=':
				switch (waiting_[2])
				{
				case u'>':
					NEW_PUNCTUATOR(SPACESHIP, 3); // <=>
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(LESS_EQ, 2); // <=
				}
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(LESS, 1); // <
			}
			break;
		}
		case u'=':
		{
			switch (waiting_[1])
			{
			case u'=':
				NEW_PUNCTUATOR(EQUAL, 2); // ==
				break;
			case u'>':
				NEW_PUNCTUATOR(RETURN, 2); // =>
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(ASSIGN, 1); // =
			}
			break;
		}
		case u'>':
		{
			switch (waiting_[1])
			{
			case u'=':
				NEW_PUNCTUATOR(GREATER_EQ, 2); // >=
				break;
			case u'>':
				switch (waiting_[2])
				{
				case u'=':
					NEW_PUNCTUATOR(SHIFT_RIGHT_AS, 3); // >>=
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(SHIFT_RIGHT, 2); // >>
				}
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(GREATER, 1); // >
			}
			break;
		}
		case u'?':
		{
			NEW_PUNCTUATOR(QUESTION, 1); // ?
			break;
		}
		case u'@':
		{
			NEW_PUNCTUATOR(AT, 1); // @
			break;
		}
		case u'[':
		{
			NEW_PUNCTUATOR(SQ_BRACKET_LEFT, 1); // [
			break;
		}
		case u'\\':
		{
			// We shouldn't be here: a backslash should initiate an escape sequence, not a punctuator
			NEW_PUNCTUATOR(BACKSLASH, 1);
			break;
		}
		case u']':
		{
			NEW_PUNCTUATOR(SQ_BRACKET_RIGHT, 1); // ]
			break;
		}
		case u'^':
		{
			switch (waiting_[1])
			{
			case u'=':
				NEW_PUNCTUATOR(XOR_AS, 2); // ^=
				break;
			case u'^':
				switch (waiting_[2])
				{
				case u'=':
					NEW_PUNCTUATOR(POW_AS, 3); // ^^=
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(POW, 2); // ^^
				}
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(XOR, 1); // ^
			}
			break;
		}
		case u'`':
		{
			NEW_PUNCTUATOR(APOSTROPHE, 1); // `
			break;
		}
		case u'{':
		{
			NEW_PUNCTUATOR(BRACKET_LEFT, 1); // {
			break;
		}
		case u'|':
		{
			switch (waiting_[1])
			{
			case u'=':
				NEW_PUNCTUATOR(BIT_OR_AS, 2); // |=
				break;
			case u'|':
				switch (waiting_[2])
				{
				case u'=':
					NEW_PUNCTUATOR(OR_AS, 3); // ||=
					break;
				case u'\0':
					if (!is_end) return;
					[[fallthrough]];
				default:
					NEW_PUNCTUATOR(OR, 2); // ||
				}
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(BIT_OR, 1); // |
			}
			break;
		}
		case u'}':
		{
			NEW_PUNCTUATOR(BRACKET_RIGHT, 1); // }
			break;
		}
		case u'~':
		{
			switch (waiting_[1])
			{
			case u'=':
				NEW_PUNCTUATOR(COMPL_AS, 2); // ~=
				break;
			case u'\0':
				if (!is_end) return;
				[[fallthrough]];
			default:
				NEW_PUNCTUATOR(COMPL, 1); // ~
			}
			break;
		}
		default:
		{
			// We shouldn't be here: I believe I added every possible punctuator from is_punctuator() in the switch
			std::shared_ptr<IdToken> token = std::make_shared<IdToken>();
			token->name = waiting_[0];
			tokens_.push_back(token);
			waiting_.erase(0);
		}
		}
	}
	// If we are here, it means that the waiting_ is empty and we're ready for the next token
	waiting_status_ = EWaitingStatus::NONE;

	#undef NEW_PUNCTUATOR_
	#undef NEW_PUNCTUATOR
	#undef NEW_PUNCTUATOR
	#undef NEW_RIGHT_PUNCTUATOR
}
