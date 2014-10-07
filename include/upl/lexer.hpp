#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>
#include <upl/input.hpp>
#include <upl/tokens.hpp>

#include <deque>

//======================================================================

namespace UPL {

//======================================================================
//----------------------------------------------------------------------
//======================================================================

class Lexer
{
public:
	explicit Lexer (InputStream & input, Error::Reporter & reporter);

	bool eoi () const {return m_cur_tok.is(TT::EOI);}
	bool error () const {return m_has_error;}

	Token const & curr () const {return m_cur_tok;}

	bool pop ();

private:
	InputStream & m_input;
	Error::Reporter & m_reporter;
	Token m_cur_tok;
	bool m_has_error;
	std::deque<Char> m_buffer;

	bool consume_whitespace();
	bool consume_comment();
	bool pop_keyword();
	bool pop_identifier();
	bool pop_bool_literal();
	bool pop_real_literal();
	bool pop_int_literal();
	bool pop_string_literal();
	bool pop_separator();
	bool pop_operator();

	bool has_more_input();
	Char read_char();
	void unread_string(String str);
	void unread_char(Char c);

	/* constants */
	const Char COMMENT_START = '#';
};

//======================================================================

}	// namespace UPL

//======================================================================
