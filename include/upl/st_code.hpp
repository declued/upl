#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/definitions.hpp>

#include <set>

//======================================================================

namespace UPL {
	namespace Type {
	
//======================================================================
//  This file implements the ST-Code codec and a container for them.
//  (ST-Code: Sertialized Type Code. A compact serialized form for the
// types defined in the language.)
//======================================================================

typedef uint32_t ID;
typedef uint32_t Size;

//----------------------------------------------------------------------

#define TAG_ENUM(v,e,s,b0,b1,b2,b3,b4)			e = v,
enum class Tag : uint8_t { UPL_PRIVATE__TYPE_TAGS(TAG_ENUM) };
#undef  TAG_ENUM

#define TAG_COUNT(v,e,s,b0,b1,b2,b3,b4)			+1
int const TagCount = UPL_PRIVATE__TYPE_TAGS(TAG_COUNT);
#undef  TAG_COUNT

static_assert (TagCount < 64, "Can't accomodate more than 6 bits worth of tags.");

//----------------------------------------------------------------------

struct TagInfo_t
{
	Tag tag;
	char const * name;
	bool is_basic;		// Is this type stand-alone (doesn't depend on any other type, e.g. "int" vs. "array" which requires specifying "array of what.")
	bool is_scalar;		// Does this hold only one value? And yes, strings are scalar.
	bool is_composite;	// Is this a "product" type (e.g. structs in C.)
	bool is_collection;	// Is this a collection of same-type values (e.g. array and strings. Yes, strings.)
	bool is_callable;		// Can you call an instance of this type, as in a function?
};

//----------------------------------------------------------------------

TagInfo_t TagInfo (Tag tag);
inline uint8_t TagToInt (Tag tag) {return int(tag);}
inline Tag IntToTag (uint8_t i) {return Tag(i);}

//======================================================================

typedef std::basic_string<uint8_t> STIR;	// ST Intermediate Representation!

//----------------------------------------------------------------------

class STCode
{
public:
	static uint8_t const msc_ConstnessBit = (1 << 7);

public:
	inline static uint8_t SerializeTag (Tag tag, bool is_const);
	inline static Tag GetTag (uint8_t serialized_tag);
	inline static bool IsValid (uint8_t serialized_tag);
	inline static bool IsConst (uint8_t serialized_tag);

	inline static STIR SerializeInt (uint32_t v);

	inline static STIR MakeBasic (bool is_const, Tag tag);
	inline static STIR MakeVariant (bool is_const, std::set<ID> const & allowed_types);
	inline static STIR MakeArray (bool is_const, Size size, ID type);
	inline static STIR MakeVector (bool is_const, ID type);
	inline static STIR MakeMap (bool is_const, ID key_type, ID value_type);
	inline static STIR MakeTuple (bool is_const, std::vector<ID> const & field_types);
	inline static STIR MakePackage (bool is_const, std::vector<ID> const & field_types);
	inline static STIR MakeFuction (bool is_const, ID return_type, std::vector<ID> const & param_types);

private:
	inline static uint8_t Qrtt (uint32_t v, int quartet);	// Zero is the low-order 4 bits
};

//----------------------------------------------------------------------
//======================================================================

	}	// namespace Types
}	// namespace UPL

//======================================================================
//======================================================================

namespace UPL {
	namespace Type {
	
//======================================================================

inline uint8_t STCode::SerializeTag (Tag tag, bool is_const)
{
	uint8_t ret = TagToInt (tag);
	if (is_const)
		ret |= msc_ConstnessBit;
	return ret;
}

//----------------------------------------------------------------------

inline Tag STCode::GetTag (uint8_t serialized_tag)
{
	return IntToTag (serialized_tag & (msc_ConstnessBit - 1));
}

//----------------------------------------------------------------------

inline bool STCode::IsValid (uint8_t serialized_tag)
{
	return GetTag(serialized_tag) != Tag::INVALID;
}

//----------------------------------------------------------------------

inline bool STCode::IsConst (uint8_t serialized_tag)
{
	return 0 != (serialized_tag & msc_ConstnessBit);
}

//----------------------------------------------------------------------

inline STIR STCode::SerializeInt (uint32_t v)
{
	if (v < 8)
		return STIR({uint8_t(v)});
	else if (v < 0x10)
		return STIR({uint8_t(7+1), Qrtt(v,0)});
	else if (v < 0x100)
		return STIR({uint8_t(7+2), Qrtt(v,1), Qrtt(v,0)});
	else if (v < 0x1000)
		return STIR({uint8_t(7+3), Qrtt(v,2), Qrtt(v,1), Qrtt(v,0)});
	else if (v < 0x10000)
		return STIR({uint8_t(7+4), Qrtt(v,3), Qrtt(v,2), Qrtt(v,1), Qrtt(v,0)});
	else if (v < 0x100000)
		return STIR({uint8_t(7+5), Qrtt(v,4), Qrtt(v,3), Qrtt(v,2), Qrtt(v,1), Qrtt(v,0)});
	else if (v < 0x1000000)
		return STIR({uint8_t(7+6), Qrtt(v,5), Qrtt(v,4), Qrtt(v,3), Qrtt(v,2), Qrtt(v,1), Qrtt(v,0)});
	else if (v < 0x10000000)
		return STIR({uint8_t(7+7), Qrtt(v,6), Qrtt(v,5), Qrtt(v,4), Qrtt(v,3), Qrtt(v,2), Qrtt(v,1), Qrtt(v,0)});
	else
		return STIR({uint8_t(7+8), Qrtt(v,7), Qrtt(v,6), Qrtt(v,5), Qrtt(v,4), Qrtt(v,3), Qrtt(v,2), Qrtt(v,1), Qrtt(v,0)});
}

//----------------------------------------------------------------------

inline STIR STCode::MakeBasic (bool is_const, Tag tag)
{
	assert (TagInfo(tag).is_basic);

	return STIR({SerializeTag (tag, is_const)});
}

//----------------------------------------------------------------------

inline STIR STCode::MakeVariant (bool is_const, std::set<ID> const & allowed_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Variant, is_const);
	ret += SerializeInt (uint32_t(allowed_types.size()));
	for (auto id : allowed_types)
		ret += SerializeInt (id);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeArray (bool is_const, Size size, ID type)
{
	STIR ret;

	ret += SerializeTag (Tag::Array, is_const);
	ret += SerializeInt (size);
	ret += SerializeInt (type);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeVector (bool is_const, ID type)
{
	STIR ret;

	ret += SerializeTag (Tag::Vector, is_const);
	ret += SerializeInt (type);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeMap (bool is_const, ID key_type, ID value_type)
{
	STIR ret;

	ret += SerializeTag (Tag::Map, is_const);
	ret += SerializeInt (key_type);
	ret += SerializeInt (value_type);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeTuple (bool is_const, std::vector<ID> const & field_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Tuple, is_const);
	ret += SerializeInt (uint32_t(field_types.size()));
	for (auto id : field_types)
		ret += SerializeInt (id);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakePackage (bool is_const, std::vector<ID> const & field_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Package, is_const);
	ret += SerializeInt (uint32_t(field_types.size()));
	for (auto id : field_types)
		ret += SerializeInt (id);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeFuction (bool is_const, ID return_type, std::vector<ID> const & param_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Tuple, is_const);
	ret += SerializeInt (return_type);
	ret += SerializeInt (uint32_t(param_types.size()));
	for (auto id : param_types)
		ret += SerializeInt (id);

	return ret;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

inline uint8_t STCode::Qrtt (uint32_t v, int quartet)
{
	return (v >> (4 * quartet)) & 0x0F;
}

//======================================================================

	}	// namespace Types
}	// namespace UPL

//======================================================================
