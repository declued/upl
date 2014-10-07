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
	String number;
	Location location;
	enum {
		INTEGER_PART,
		FRACTIONAL_PART_FIRST,
		FRACTIONAL_PART_REST,
		EXPONENT_SIGN,
		EXPONENT_VALUE_FIRST,
		EXPONENT_VALUE_REST,
		DONE_INTEGER,
		DONE_REAL,
		ERROR
	} state = INTEGER_PART;

	if (!IsDigit(current_char()))
		return false;

	location = current_location();

	while (state != DONE_INTEGER && state != DONE_REAL && state != ERROR) {
		char c = current_char();
		switch (state) {
		case INTEGER_PART:
			if (IsDigit(c)) {
				number += c;
				consume_one_char();
			}
			else if (c == '.') {
				number += c;
				consume_one_char();
				state = FRACTIONAL_PART_FIRST;
			}
			else {
				state = DONE_INTEGER;
			}
			break;
		case FRACTIONAL_PART_FIRST:
			if (IsDigit(c)) {
				number += c;
				consume_one_char();
				state = FRACTIONAL_PART_REST;
			}
			else {
				state = ERROR;
			}
			break;
		case FRACTIONAL_PART_REST:
			if (IsDigit(c)) {
				number += c;
				consume_one_char();
			}
			else if (c == 'e') {
				number += c;
				consume_one_char();
				state = EXPONENT_SIGN;
			}
			else {
				state = DONE_REAL;
			}
			break;
		case EXPONENT_SIGN:
			if (c == '+' || c == '-') {
				number += c;
				consume_one_char();
			}
			state = EXPONENT_VALUE_FIRST;
			break;
		case EXPONENT_VALUE_FIRST:
			if (IsDigit(c)) {
				number += c;
				consume_one_char();
				state = EXPONENT_VALUE_REST;
			}
			else {
				state = ERROR;
			}
			break;
		case EXPONENT_VALUE_REST:
			if (IsDigit(c)) {
				number += c;
				consume_one_char();
			}
			else {
				state = DONE_REAL;
			}
			break;
		}
	}

	if (state == DONE_INTEGER) {
		Int value = stoll(number);
		m_cur_tok = Token(TT::IntLiteral, location, number, value);
	}
	else if (state == DONE_REAL) {
		Real value = stod(number);
		m_cur_tok = Token(TT::RealLiteral, location, number, value);
	}
	else if (state == ERROR) {
		m_cur_tok = Token(TT::Error, location, number);
	}

	return true;
}

bool Lexer::pop_string_literal()
{
	String uncooked;
	String value;
	Location location;
	bool escape = false;
	bool error = false;
	bool done = false;

	if (current_char() != '"')
		return false;

	location = current_location();
	uncooked += current_char();
	consume_one_char();

	while (!done && !error) {
		char c = current_char();
		if (escape) {
			if (c == 'n') {
				value += '\n';
			}
			else if (c == 't') {
				value += '\t';
			}
			else {
				value += c;
			}
			uncooked += c;
			consume_one_char();
			escape = false;
		}
		else if (IsNewline(c) || c == 0) {
			error = true;
		}
		else if (c == '"') {
			uncooked += c;
			consume_one_char();
			done = true;
		}
		else if (c == '\\') {
			uncooked += c;
			consume_one_char();
			escape = true;
		}
		else {
			value += c;
			uncooked += c;
			consume_one_char();
		}
	}

	if (error) {
		m_cur_tok = Token(TT::Error, location, uncooked);
	}
	else {
		m_cur_tok = Token(TT::StrLiteral, location, uncooked, value);
	}

	return true;
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
