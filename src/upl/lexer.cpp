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

	if (pop_keyword() || pop_identifier() || pop_bool_literal() ||
		pop_real_literal() || pop_int_literal() || pop_string_literal() ||
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
		Char c = read_char();
		if (!IsWhitespace(c)) {
			unread_char(c);
			break;
		}

		consumed_input = true;
	}

	return consumed_input;
}

bool Lexer::consume_comment()
{
	Char c = read_char();
	if (c != COMMENT_START) {
		unread_char(c);
		return false;
	}

	while (has_more_input()) {
		c = read_char();
		if (IsNewline(c))
			break;
	}

	return true;
}

bool Lexer::pop_keyword()
{
	//
}

bool Lexer::pop_identifier()
{
	//
}

bool Lexer::pop_bool_literal()
{
	//
}

bool Lexer::pop_real_literal()
{
	//
}

bool Lexer::pop_int_literal()
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
	if (m_buffer.size() == 0) {
		if (!m_input.pop())
			return false;
		m_buffer.push_back(m_input.curr());
	}

	return true;
}

Char Lexer::read_char()
{
	if (!has_more_input())
		return 0;

	Char result = m_buffer.front();
	m_buffer.pop_front();

	return result;
}

void Lexer::unread_string(String str)
{
	for (String::reverse_iterator it = str.rbegin(); it != str.rend(); it++)
		unread_char(*it);
}

void Lexer::unread_char(Char c)
{
	m_buffer.push_front(c);
}

//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
