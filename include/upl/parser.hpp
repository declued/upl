#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>
#include <upl/lexer.hpp>
#include <upl/ast.hpp>

//======================================================================

namespace UPL {

//======================================================================

class Parser
{
public:
	Parser (Lexer & lexer, Error::Reporter & reporter);

	// Non-copyable and non-movable (for now.)
	Parser (Parser const &) = delete;
	Parser (Parser &&) = delete;
	Parser & operator = (Parser const &) = delete;
	Parser & operator = (Parser &&) = delete;

	AST::Node * parse ();

private:
	AST::Program * parseProgram ();
	AST::Statement * parseStatement ();
	AST::Declaration * parseDeclaration ();
	AST::Expression * parseExpression ();
	// .
	// .
	// .

private:
	Lexer & m_lexer;
	Error::Reporter & m_reporter;
};

//======================================================================
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
