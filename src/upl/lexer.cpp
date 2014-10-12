//======================================================================

#include <upl/lexer.hpp>
#include <cstring>

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

	if (m_cur_tok.is(TT::EOI))
		return false;

	while (consume_whitespace() || consume_comment());

	if (pop_name() || pop_numeric_literal() | pop_string_literal() ||
		pop_separator_or_operator())
	{
		token_popped = true;
	}

	if (!token_popped && !m_input.eoi()) {
		Location location = m_input.location();
		Char c = m_input.curr();
		m_input.pop();

		String value;
		value += c;

		m_cur_tok = Token(TT::Error, location, value);
		token_popped = true;
	}

	if (!token_popped) {
		m_cur_tok = Token(TT::EOI, m_input.location(), L"");
		token_popped = true;
	}

	return token_popped;
}

//----------------------------------------------------------------------

bool Lexer::consume_whitespace()
{
	bool consumed_input = false;

	while (!m_input.eoi()) {
		Char c = m_input.curr();
		if (!IsWhitespace(c))
			break;

		m_input.pop();
		consumed_input = true;
	}

	return consumed_input;
}

//----------------------------------------------------------------------

bool Lexer::consume_comment()
{
	if (!IsCommentStarter(m_input.curr()))
		return false;

	while (!m_input.eoi()) {
		Char c = m_input.curr();
		if (IsNewline(c))
			break;

		m_input.pop();
	}

	return true;
}

//----------------------------------------------------------------------

bool Lexer::pop_name()
{
	String name;
	Location location;

	if (!IsIdentStarter(m_input.curr()))
		return false;

	/* save location and read the name */
	location = m_input.location();
	while (!m_input.eoi() && IsIdentContinuer(m_input.curr())) {
		name += m_input.curr();
		m_input.pop();
	}

	/* check for bool literals */
	if (IsFalseLiteral(name)) {
		m_cur_tok = Token(TT::BoolLiteral, location, name, false);
		return true;
	}
	else if (IsTrueLiteral(name)) {
		m_cur_tok = Token(TT::BoolLiteral, location, name, true);
		return true;
	}

	/* check for keywords */
	TT keyword_token = StringToKeyword(name);
	if (TT::Error != keyword_token)
	{
		m_cur_tok = Token(keyword_token, location, name);
		return true;
	}

	/* otherwise it is a simple identifier */
	m_cur_tok = Token(TT::Identifier, location, name);
	return true;
}

//----------------------------------------------------------------------

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

	if (!IsDigit(m_input.curr()))
		return false;

	location = m_input.location();

	while (state != DONE_INTEGER && state != DONE_REAL && state != ERROR) {
		char c = m_input.curr();
		switch (state) {
		case INTEGER_PART:
			if (IsDigit(c)) {
				number += c;
				m_input.pop();
			}
			else if (c == UPL_PRIVATE__FRACTIONAL_SEP) {
				number += c;
				m_input.pop();
				state = FRACTIONAL_PART_FIRST;
			}
			else {
				state = DONE_INTEGER;
			}
			break;
		case FRACTIONAL_PART_FIRST:
			if (IsDigit(c)) {
				number += c;
				m_input.pop();
				state = FRACTIONAL_PART_REST;
			}
			else {
				state = ERROR;
			}
			break;
		case FRACTIONAL_PART_REST:
			if (IsDigit(c)) {
				number += c;
				m_input.pop();
			}
			else if (c == UPL_PRIVATE__EXPONENT_SEP) {
				number += c;
				m_input.pop();
				state = EXPONENT_SIGN;
			}
			else {
				state = DONE_REAL;
			}
			break;
		case EXPONENT_SIGN:
			if (c == UPL_PRIVATE__POSITIVE_SIGN ||
				c == UPL_PRIVATE__NEGATIVE_SIGN) {
				number += c;
				m_input.pop();
			}
			state = EXPONENT_VALUE_FIRST;
			break;
		case EXPONENT_VALUE_FIRST:
			if (IsDigit(c)) {
				number += c;
				m_input.pop();
				state = EXPONENT_VALUE_REST;
			}
			else {
				state = ERROR;
			}
			break;
		case EXPONENT_VALUE_REST:
			if (IsDigit(c)) {
				number += c;
				m_input.pop();
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

//----------------------------------------------------------------------

bool Lexer::pop_string_literal()
{
	String uncooked;
	String value;
	Location location;
	bool escape = false;
	bool error = false;
	bool done = false;

	if (!IsStringDelimiter(m_input.curr()))
		return false;

	location = m_input.location();
	uncooked += m_input.curr();
	m_input.pop();

	while (!done && !error) {
		char c = m_input.curr();
		if (escape) {
			value += EscapeCharacter(c);
			uncooked += c;
			m_input.pop();
			escape = false;
		}
		else if (IsNewline(c) || c == 0) {
			error = true;
		}
		else if (IsStringDelimiter(c)) {
			uncooked += c;
			m_input.pop();
			done = true;
		}
		else if (IsStringEscapeCharacter(c)) {
			uncooked += c;
			m_input.pop();
			escape = true;
		}
		else {
			value += c;
			uncooked += c;
			m_input.pop();
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

//----------------------------------------------------------------------

bool Lexer::pop_separator_or_operator()
{
	TT token_type = TT::Empty;
	Location location = m_input.location();
	String uncooked;

	/* detect single character separators */
	switch (m_input.curr()) {
	case UPL_PRIVATE__OPEN_BRACKET:
		token_type = TT::OpenBracket;
		break;
	case UPL_PRIVATE__CLOSE_BRACKET:
		token_type = TT::CloseBracket;
		break;
	case UPL_PRIVATE__OPEN_PAREN:
		token_type = TT::OpenParen;
		break;
	case UPL_PRIVATE__CLOSE_PAREN:
		token_type = TT::CloseParen;
		break;
	case UPL_PRIVATE__ARGUMENT_SEP:
		token_type = TT::ArgumentSep;
		break;
	case UPL_PRIVATE__STATEMENT_SEP:
		token_type = TT::StatementSep;
		break;
	}

	if (token_type != TT::Empty) {
		uncooked += m_input.curr();
		m_input.pop();

		m_cur_tok = Token(token_type, location, uncooked);
		return true;
	}

	/* detect multi character separators and operators */
	while (!m_input.eoi() &&
		   strchr(UPL_PRIVATE__OPERATOR_CHAR_SET, m_input.curr()) != NULL) {
		uncooked += m_input.curr();
		m_input.pop();
	}

	if (uncooked == UPL_PRIVATE__ASSIGNMENT)
		token_type = TT::Assignment;
	else if (uncooked == UPL_PRIVATE__RETURNS_SEP)
		token_type = TT::ReturnsSep;
	else if (uncooked.length() != 0)
		token_type = TT::Operator;

	if (token_type != TT::Empty) {
		m_cur_tok = Token(token_type, location, uncooked);
		return true;
	}
	else {
		return false;
	}
}

//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
