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
#undef TT_ENUM

#define TT_COUNT(e)		+1
static int const TTCount = UPL_PRIVATE__TOKEN_TYPES(TT_COUNT);
#undef TT_COUNT

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//======================================================================

char const * TokenType2String (TT token_type);

//======================================================================

class Token
{
public:
	Token () : m_type (TT::Empty) {}
	Token (TT type, Location const & location) : m_type (type), m_loc (location) {}
	Token (TT type, Location const & location, String uncooked_value);
	Token (TT type, Location const & location, String uncooked_value, Bool bool_value);
	Token (TT type, Location const & location, String uncooked_value, Int int_value);
	Token (TT type, Location const & location, String uncooked_value, Real real_value);
	Token (TT type, Location const & location, String uncooked_value, String string_value);

	bool is (TT tok_type) const {return m_type == tok_type;}
	bool isnt (TT tok_type) const {return m_type != tok_type;}

	TT type () const {return m_type;}
	char const * typeStr () const {return TokenType2String(type());}
	Location const & location () const {return m_loc;}
	String const & uncookedValue () const {return m_uncooked;}

	Bool const & valueBool () const {return m_value.b;}
	Int const & valueInt () const {return m_value.i;}
	Real const & valueReal () const {return m_value.r;}
	String const & valueString () const {return m_value.s;}

private:
	TT m_type;
	Location m_loc;
	String m_uncooked;
	// A better (smaller) alternative is Boost.Variant instead of struct
	struct {
		String s;
		Real r;
		Int i;
		Bool b;
	} m_value;
};

//======================================================================

}	// namespace UPL

//======================================================================
