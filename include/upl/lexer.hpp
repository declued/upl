#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>
#include <upl/input.hpp>
#include <upl/tokens.hpp>

//======================================================================

namespace UPL {

//======================================================================

class Lexer
{
public:
	explicit Lexer (InputStream & input, Error::Reporter & reporter);

	// Non-copyable and non-movable (for now.)
	Lexer (Lexer const &) = delete;
	Lexer (Lexer &&) = delete;
	Lexer & operator = (Lexer const &) = delete;
	Lexer & operator = (Lexer &&) = delete;

	bool eoi () const {return m_cur_tok.is(TT::EOI);}
	bool error () const {return m_has_error;}

	Token const & curr () const {return m_cur_tok;}

	bool pop ();

private:
	bool consume_whitespace();
	bool consume_comment();
	bool pop_name();
	//bool pop_bool_literal();
	bool pop_numeric_literal();
	bool pop_string_literal();
	bool pop_separator_or_operator();

private:
	InputStream & m_input;
	Error::Reporter & m_reporter;
	Token m_cur_tok;
	bool m_has_error;
};

//======================================================================
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
