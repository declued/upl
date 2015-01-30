//======================================================================

#include <upl/parser.hpp>

//======================================================================

namespace UPL {

//======================================================================

Parser::Parser (Lexer & lexer, Error::Reporter & reporter, Type::STContainer & type_container)
	: m_type_container (type_container)
	, m_lexer (lexer)
	, m_reporter(reporter)
{
}

//----------------------------------------------------------------------

Ptr<AST::Program> Parser::parseProgram ()
{
	AST::Program *program = new AST::Program;
	AST::Statement *statement = nullptr;

	while ((statement = parseStatement()) != nullptr) {
		program->statements.push_back(statement);
	}

	return program;
}

//----------------------------------------------------------------------

Ptr<AST::Statement> Parser::parseStatement ()
{
	AST::Statement *statement = nullptr;

	statement = parseDeclaration();
	if (statement == nullptr) {
		statement = parseExpression();
	}

	return statement;
}

//----------------------------------------------------------------------

Ptr<AST::Declaration> Parser::parseDeclaration ()
{
	AST::Declaration *declaration = nullptr;

	Token declarator = m_lexer.curr();
	if (!declarator.is(TT::KeywordBool) &&
		!declarator.is(TT::KeywordInt) &&
		!declarator.is(TT::KeywordReal))
	{
		return nullptr;
	}

	m_lexer.pop();

	Token identifier = m_lexer.curr();
	if (!identifier.is(TT::Identifier))
	{
		return nullptr;
	}

	m_lexer.pop();

	if (!m_lexer.curr().is(TT::Assignment))
	{
		return nullptr;
	}

	m_lexer.pop();

	AST::Expression *expression = parseExpression();
	if (expression == nullptr)
	{
		return nullptr;
	}

	if (!m_lexer.curr().is(TT::StatementSep))
	{
		return nullptr;
	}

	m_lexer.pop();

	return declaration;
}

//----------------------------------------------------------------------

Ptr<AST::Expression> Parser::parseExpression ()
{
	AST::Expression *expression = nullptr;

	Token literal = m_lexer.curr();
	if (!literal.is(TT::BoolLiteral) &&
		!literal.is(TT::IntLiteral) &&
		!literal.is(TT::RealLiteral) &&
		!literal.is(TT::StrLiteral))
	{
		return nullptr;
	}

	m_lexer.pop();

	return expression;
}

//----------------------------------------------------------------------

//======================================================================

}	// namespace UPL

//======================================================================
