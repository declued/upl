#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>
#include <upl/lexer.hpp>
#include <upl/ast.hpp>

//======================================================================

namespace UPL {

//======================================================================

using AST::Ptr;

//======================================================================

class Parser
{
public:
	Parser (Lexer & lexer, Error::Reporter & reporter, Type::STContainer & type_container);

	// Non-copyable and non-movable (for now.)
	Parser (Parser const &) = delete;
	Parser (Parser &&) = delete;
	Parser & operator = (Parser const &) = delete;
	Parser & operator = (Parser &&) = delete;

	Ptr<AST::Node> parse ();

private:
	Ptr<AST::Program> parseProgram ();
	Ptr<AST::Statement> parseStatement ();
	Ptr<AST::Declaration> parseDeclaration ();
	Ptr<AST::Expression> parseExpression ();
	// .
	// .
	// .
	
	bool haveMoreTokens () const {return m_lexer.curr().isnt(TT::EOI) && m_lexer.curr().isnt(TT::Error);}

private:
	Type::STContainer & m_type_container;
	Lexer & m_lexer;
	Error::Reporter & m_reporter;
};

//======================================================================
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
