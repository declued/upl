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
	Char m_current_char;
	Location m_current_location;

	bool consume_whitespace();
	bool consume_comment();
	bool pop_name();
	bool pop_bool_literal();
	bool pop_numeric_literal();
	bool pop_string_literal();
	bool pop_separator_or_operator();

	bool has_more_input();
	Char current_char();
	Location current_location();
	void ensure_current_char();
	void consume_one_char();

	/* constants */
	const Char COMMENT_START = '#';
	const String FALSE_STR = L"false";
	const String TRUE_STR = L"true";
	static const int KEYWORD_COUNT = 6;
	const std::pair<String, TT> KEYWORDS[KEYWORD_COUNT] = {
		{L"def", TT::KeywordDef},
		{L"var", TT::KeywordVar},
		{L"bool", TT::KeywordBool},
		{L"int", TT::KeywordInt},
		{L"real", TT::KeywordReal},
		{L"func", TT::KeywordFunc}
	};
};

//======================================================================

}	// namespace UPL

//======================================================================
