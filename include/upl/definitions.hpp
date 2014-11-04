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
#define UPL_PRIVATE__OPERATOR_CHAR_SET			L"?:|^&!=<>*/%~-+"
#define UPL_PRIVATE__ASSIGNMENT					'='
#define UPL_PRIVATE__RETURNS_SEP				L"->"
#define UPL_PRIVATE__FRACTIONAL_SEP				'.'
#define UPL_PRIVATE__EXPONENT_SEP				'e'
#define UPL_PRIVATE__POSITIVE_SIGN				'+'
#define UPL_PRIVATE__NEGATIVE_SIGN				'-'

//----------------------------------------------------------------------
//======================================================================

#define UPL_PRIVATE__POINTER_SIZE				8

//----------------------------------------------------------------------

#define UPL_PRIVATE__TYPE_TAGS(action)									\
/*TagValue,Enum,Name,IsScalar,IsComposite,IsCollection,IsCallable */	\
	action ( 0, INVALID , "(ERR)"   , true,false,false,false)			\
	action ( 1, Nil     , "nil"     , true,false,false,false)			\
	action ( 2, Bool    , "bool"    , true,false,false,false)			\
	action ( 3, Byte    , "byte"    , true,false,false,false)			\
	action ( 4, Char    , "char"    , true,false,false,false)			\
	action ( 5, Int     , "int"     , true,false,false,false)			\
	action ( 6, Real    , "real"    , true,false,false,false)			\
	action ( 7, String  , "string"  , true,false,false,false)			\
	action ( 8, Any     , "any"     , true,false,false,false)			\
	action ( 9, Variant , "variant" , true,false,false,false)			\
	action (10, Array   , "array"   ,false,false, true,false)			\
	action (11, Map     , "map"     ,false,false, true,false)			\
	action (12, Tuple   , "tuple"   ,false, true,false,false)			\
	action (13, Package , "package" ,false, true,false,false)			\
	action (14, Function, "function", true,false,false, true)

//======================================================================

namespace UPL {

//======================================================================
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================

