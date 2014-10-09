//======================================================================

#include <upl/lexer.hpp>

//======================================================================

namespace UPL {

//======================================================================

#define TT_NAME_STRINGS(e)		UPL_STRINGIZE(e),
static char const * s_token_names [TTCount] = { UPL_PRIVATE__TOKEN_TYPES(TT_NAME_STRINGS) };
#undef  TT_NAME_STRINGS

//----------------------------------------------------------------------

#define KEYWORD_NAME_STRINGS(e, s)	{s, TT::e},
static struct {
	Char const * str;
	TT tt;
} s_keyword_strs_and_tt [KeywordCount] = { UPL_PRIVATE__KEYWORDS(KEYWORD_NAME_STRINGS) };
#undef  KEYWORD_NAME_STRINGS

//======================================================================

char const * TokenTypeToString (TT token_type)
{
	if (int(token_type) >= 0 && int(token_type) < TTCount)
		return s_token_names[int(token_type)];
	else
		return nullptr;
}

//----------------------------------------------------------------------

Char const * KeywordToString (TT keyword_token_type)
{
	for (int i = 0; i < KeywordCount; ++i)
		if (s_keyword_strs_and_tt[i].tt == keyword_token_type)
			return s_keyword_strs_and_tt[i].str;
	return nullptr;
}

//----------------------------------------------------------------------
// TODO: This can be faster using a hashtable or even a sorted table of strings or something.
TT StringToKeyword (String const & kw_str)
{
	for (int i = 0; i < KeywordCount; ++i)
		if (kw_str == s_keyword_strs_and_tt[i].str)
			return s_keyword_strs_and_tt[i].tt;
	return TT::Error;
}

//----------------------------------------------------------------------

bool TokenIsKeyword (TT token_type)
{
	return nullptr != KeywordToString(token_type);
}

//----------------------------------------------------------------------

bool StringIsKeyword (String const & kw_str)
{
	return TT::Error != StringToKeyword(kw_str);
}

//----------------------------------------------------------------------

bool IsCommentStarter (Char c)
{
	return c == UPL_PRIVATE__COMMENT_START_CHAR;
}

//----------------------------------------------------------------------

bool IsTrueLiteral (String const & str)
{
	return str == UPL_PRIVATE__LITERAL_TRUE_STR;
}

//----------------------------------------------------------------------

bool IsFalseLiteral (String const & str)
{
	return str == UPL_PRIVATE__LITERAL_FALSE_STR;
}

//----------------------------------------------------------------------
//======================================================================
//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
