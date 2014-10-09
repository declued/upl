#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>
#include <upl/definitions.hpp>

//======================================================================

namespace UPL {

//======================================================================

#define TT_ENUM(e)		e,
enum class TT { UPL_PRIVATE__TOKEN_TYPES(TT_ENUM) };
#undef  TT_ENUM

#define TT_COUNT(e)		+1
int const TTCount = UPL_PRIVATE__TOKEN_TYPES(TT_COUNT);
#undef  TT_COUNT

//----------------------------------------------------------------------

#define KEYWORD_COUNT(e, s)	+1
int const KeywordCount = UPL_PRIVATE__KEYWORDS(KEYWORD_COUNT);
#undef  KEYWORD_COUNT

//----------------------------------------------------------------------
//======================================================================

char const * TokenTypeToString (TT token_type);

Char const * KeywordToString (TT keyword_token_type);
TT StringToKeyword (String const & kw_str);
bool TokenIsKeyword (TT token_type);
bool StringIsKeyword (String const & kw_str);

bool IsCommentStarter (Char c);
bool IsTrueLiteral (String const & str);
bool IsFalseLiteral (String const & str);

//======================================================================

class Token
{
public:
	Token () : m_type (TT::Empty) {}
	Token (TT type, Location const & location) : m_type (type), m_loc (location) {}
	Token (TT type, Location const & location, String uncooked_value)
		: m_type (type), m_loc (location), m_uncooked (std::move(uncooked_value)) {}
	Token (TT type, Location const & location, String uncooked_value, Bool bool_value)
		: m_type (type), m_loc (location), m_uncooked (std::move(uncooked_value))
	{m_value.b = bool_value;}
	Token (TT type, Location const & location, String uncooked_value, Int int_value)
		: m_type (type), m_loc (location), m_uncooked (std::move(uncooked_value))
	{m_value.i = int_value;}
	Token (TT type, Location const & location, String uncooked_value, Real real_value)
		: m_type (type), m_loc (location), m_uncooked (std::move(uncooked_value))
	{m_value.r = real_value;}
	Token (TT type, Location const & location, String uncooked_value, String string_value)
		: m_type (type), m_loc (location), m_uncooked (std::move(uncooked_value))
	{m_value.s = std::move(string_value);}

	bool is (TT tok_type) const {return m_type == tok_type;}
	bool isnt (TT tok_type) const {return m_type != tok_type;}

	TT type () const {return m_type;}
	char const * typeStr () const {return TokenTypeToString(type());}
	Location const & location () const {return m_loc;}
	String const & uncookedValue () const {return m_uncooked;}

	Bool const & valueBool () const {return m_value.b;}
	Int const & valueInt () const {return m_value.i;}
	Real const & valueReal () const {return m_value.r;}
	String const & valueString () const {return m_value.s;}

	bool isKeyword () const {return TokenIsKeyword(type());}
	
private:
	TT m_type;
	Location m_loc;
	String m_uncooked;
	// A better (smaller) alternative is Boost.Variant instead of struct
	struct {
		String s;
		Real r = 0;
		Int i = 0;
		Bool b = false;
	} m_value;
};

//======================================================================

}	// namespace UPL

//======================================================================
