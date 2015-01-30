//======================================================================

#include <upl/parser.hpp>

//======================================================================

namespace UPL {

//======================================================================

Parser::Parser (Lexer & lexer, Error::Reporter & reporter, Type::STContainer & type_container)
	: m_type_container (type_container)
	, m_lexer (lexer)
	, m_reporter (reporter)
{
}

//----------------------------------------------------------------------

Ptr<AST::Node> Parser::parse ()
{
	return parseProgram();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

Ptr<AST::Program> Parser::parseProgram ()
{
	if (!haveMoreTokens())
		return nullptr;

	auto ret = new AST::Program ();

	while (haveMoreTokens())
		ret->addChild (parseStatement());

	return ret;
}

//----------------------------------------------------------------------

Ptr<AST::Statement> Parser::parseStatement ()
{
	return nullptr;
}

//----------------------------------------------------------------------

Ptr<AST::Declaration> Parser::parseDeclaration ()
{
	return nullptr;
}

//----------------------------------------------------------------------

Ptr<AST::Expression> Parser::parseExpression ()
{
	return nullptr;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
