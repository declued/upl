#pragma once

//======================================================================

#include <upl/common.hpp>

//======================================================================

#define UPL_PRIVATE__TOKEN_TYPES(action)							\
	action (/* 0,*/ Empty)			/**/							\
	action (/* 1,*/ Error)											\
	action (/* 2,*/ EOI)											\
	action (/* 3,*/ Whitespace)		/*   */							\
	action (/* 4,*/ Comment)		/* # */							\
	action (/* 5,*/ BoolLiteral)									\
	action (/* 6,*/ IntLiteral)										\
	action (/* 7,*/ RealLiteral)									\
	action (/* 8,*/ StrLiteral)										\
	action (/* 9,*/ Identifier)										\
	action (/*10,*/ OpenBracket)									\
	action (/*11,*/ CloseBracket)									\
	action (/*12,*/ OpenParen)										\
	action (/*13,*/ CloseParen)										\
	action (/*14,*/ Assignment)										\
	action (/*15,*/ StatementSep)									\
	action (/*16,*/ ArgumentSep)									\
	action (/*17,*/ ReturnsSep)										\
	action (/*18,*/ Operator)										\
	action (/*19,*/ KeywordDef)										\
	action (/*20,*/ KeywordVar)										\
	action (/*21,*/ KeywordBool)									\
	action (/*22,*/ KeywordInt)										\
	action (/*23,*/ KeywordReal)									\
	action (/*24,*/ KeywordFunc)

//----------------------------------------------------------------------
// The first half of these should obviously be the same as above
#define UPL_PRIVATE__KEYWORDS(action)								\
	action (KeywordDef,		L"def")									\
	action (KeywordVar,		L"var")									\
	action (KeywordBool,	L"bool")								\
	action (KeywordInt,		L"int")									\
	action (KeywordReal,	L"real")								\
	action (KeywordFunc,	L"func")

//----------------------------------------------------------------------

#define UPL_PRIVATE__COMMENT_START_CHAR			'#'
#define UPL_PRIVATE__LITERAL_FALSE_STR			L"false"
#define UPL_PRIVATE__LITERAL_TRUE_STR			L"true"
#define UPL_PRIVATE__STRING_DELIMITER			'"'
#define UPL_PRIVATE__STRING_ESCAPE_CHARACTER 	'\\'
#define UPL_PRIVATE__OPEN_BRACKET				'{'
#define UPL_PRIVATE__CLOSE_BRACKET				'}'
#define UPL_PRIVATE__OPEN_PAREN					'('
#define UPL_PRIVATE__CLOSE_PAREN				')'
#define UPL_PRIVATE__ARGUMENT_SEP				','
#define UPL_PRIVATE__STATEMENT_SEP				';'
#define UPL_PRIVATE__OPERATOR_CHAR_SET			"?:|^&!=<>*/%~-+"
#define UPL_PRIVATE__ASSIGNMENT					L"="
#define UPL_PRIVATE__RETURNS_SEP				L"->"
#define UPL_PRIVATE__FRACTIONAL_SEP				'.'
#define UPL_PRIVATE__EXPONENT_SEP				'e'
#define UPL_PRIVATE__POSITIVE_SIGN				'+'
#define UPL_PRIVATE__NEGATIVE_SIGN				'-'

//----------------------------------------------------------------------
//======================================================================

namespace UPL {

//======================================================================
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================

