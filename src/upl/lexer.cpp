//======================================================================

#include <upl/lexer.hpp>

//======================================================================

namespace UPL {

//======================================================================

Lexer::Lexer (InputStream & input, Error::Reporter & reporter)
	: m_input (input)
	, m_reporter (reporter)
	, m_cur_tok ()
	, m_has_error (false)
{
	pop ();
}

//----------------------------------------------------------------------

bool Lexer::pop ()
{
	bool token_popped = false;

	while (consume_whitespace() || consume_comment());

	if (pop_name() || pop_numeric_literal() | pop_string_literal() ||
		pop_separator() || pop_operator())
	{
		token_popped = true;
	}

	return token_popped;
}

bool Lexer::consume_whitespace()
{
	bool consumed_input = false;

	while (has_more_input()) {
		Char c = current_char();
		if (!IsWhitespace(c))
			break;

		consume_one_char();
		consumed_input = true;
	}

	return consumed_input;
}

bool Lexer::consume_comment()
{
	if (current_char() != COMMENT_START)
		return false;

	while (has_more_input()) {
		Char c = current_char();
		if (IsNewline(c))
			break;

		consume_one_char();
	}

	return true;
}

bool Lexer::pop_name()
{
	String name;
	Location location;

	if (!IsIdentStarter(current_char()))
		return false;

	/* save location and read the name */
	location = current_location();
	while (has_more_input() && IsIdentContinuer(current_char())) {
		name += current_char();
		consume_one_char();
	}

	/* check for bool literals */
	if (name == FALSE_STR) {
		m_cur_tok = Token(TT::BoolLiteral, location, name, false);
		return true;
	}
	else if (name == TRUE_STR) {
		m_cur_tok = Token(TT::BoolLiteral, location, name, true);
		return true;
	}

	/* check for keywords */
	for (int i = 0; i < KEYWORD_COUNT; i++)
		if (name == KEYWORDS[i].first) {
			m_cur_tok = Token(KEYWORDS[i].second, location, name);
			return true;
		}

	/* otherwise it is a simple identifier */
	m_cur_tok = Token(TT::Identifier, location, name);
	return true;
}

bool Lexer::pop_numeric_literal()
{
	//
}

bool Lexer::pop_string_literal()
{
	//
}

bool Lexer::pop_separator()
{
	//
}

bool Lexer::pop_operator()
{
	//
}

bool Lexer::has_more_input()
{
	if (current_char() == 0)
		return false;
	else
		return true;
}

Char Lexer::current_char()
{
	ensure_current_char();
	return m_current_char;
}

Location Lexer::current_location()
{
	ensure_current_char();
	return m_current_location;
}

void Lexer::ensure_current_char()
{
	if (m_current_char == 0)
	{
		if (m_input.pop())
		{
			m_current_char = m_input.curr();
			m_current_location = m_input.location();
		}
	}
}

void Lexer::consume_one_char()
{
	m_current_char = 0;
}

//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
