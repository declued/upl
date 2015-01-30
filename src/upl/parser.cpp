//======================================================================

#include <upl/parser.hpp>

//======================================================================

namespace UPL {

//======================================================================

Parser::Parser (Lexer & lexer, Error::Reporter & reporter):
	m_lexer (lexer), m_reporter(reporter)
{
}

//----------------------------------------------------------------------

AST::Program * Parser::parseProgram ()
{
	AST::Program *program = new AST::Program;
	AST::Statement *statement = NULL;

	while ((statement = parseStatement()) != NULL) {
		program->statements.push_back(statement);
	}

	return program;
}

//----------------------------------------------------------------------

AST::Statement * Parser::parseStatement ()
{
	AST::Statement *statement = NULL;

	statement = parseDeclaration();
	if (statement == NULL) {
		statement = parseExpression();
	}

	return statement;
}

//----------------------------------------------------------------------

AST::Declaration * Parser::parseDeclaration ()
{
	AST::Declaration *declaration = NULL;

	Token declarator = m_lexer.curr();
	if (!declarator.is(TT::KeywordBool) &&
		!declarator.is(TT::KeywordInt) &&
		!declarator.is(TT::KeywordReal))
	{
		return NULL;
	}

	m_lexer.pop();

	Token identifier = m_lexer.curr();
	if (!identifier.is(TT::Identifier))
	{
		return NULL;
	}

	m_lexer.pop();

	if (!m_lexer.curr().is(TT::Assignment))
	{
		return NULL;
	}

	m_lexer.pop();

	AST::Expression *expression = parseExpression();
	if (expression == NULL)
	{
		return NULL;
	}

	if (!m_lexer.curr().is(TT::StatementSep))
	{
		return NULL;
	}

	m_lexer.pop();

	return declaration;
}

//----------------------------------------------------------------------

AST::Expression * Parser::parseExpression ()
{
	AST::Expression *expression = NULL;

	Token literal = m_lexer.curr();
	if (!literal.is(TT::BoolLiteral) &&
		!literal.is(TT::IntLiteral) &&
		!literal.is(TT::RealLiteral) &&
		!literal.is(TT::StrLiteral))
	{
		return NULL;
	}

	m_lexer.pop();

	return expression;
}

//----------------------------------------------------------------------

//======================================================================

}	// namespace UPL

//======================================================================
