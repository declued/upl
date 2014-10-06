#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>
#include <upl/input.hpp>
#include <upl/tokens.hpp>

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
};

//======================================================================

}	// namespace UPL

//======================================================================
