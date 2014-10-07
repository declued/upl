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
	action (/* 10,*/ OpenBracket)									\
	action (/* 11,*/ CloseBracket)									\
	action (/* 12,*/ OpenParen)										\
	action (/* 13,*/ CloseParen)									\
	action (/* 14,*/ Assignment)									\
	action (/* 15,*/ StatementSep)									\
	action (/* 16,*/ ArgumentSep)									\
	action (/* 17,*/ ReturnsSep)									\
	action (/* 18,*/ Operator)										\
	action (/* 19,*/ KeywordDef)									\
	action (/* 20,*/ KeywordVar)									\
	action (/* 21,*/ KeywordBool)									\
	action (/* 22,*/ KeywordInt)									\
	action (/* 23,*/ KeywordReal)									\
	action (/* 24,*/ KeywordFunc)
//	.
//	.
//	.

//----------------------------------------------------------------------
//======================================================================

namespace UPL {

//======================================================================
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================

