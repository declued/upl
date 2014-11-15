#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/definitions.hpp>

#include <set>
#include <unordered_map>

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

static_assert (TagCount < 32, "Can't accomodate more than 5 bits worth of tags.");

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
typedef std::basic_string<uint8_t> PackedST;

//----------------------------------------------------------------------

class STCode
{
public:
	static uint8_t const msc_ConstnessBit = (1 << 7);
	static uint8_t const msc_OddLengthBit = (1 << 6);

public:
	// Being "odd length" means that the least signigicant 4 bits of the last
	// byte the *packed* form is not part of the ST-code.
	static inline uint8_t SerializeTag (Tag tag, bool is_const, bool is_odd_length);
	static inline Tag GetTag (uint8_t serialized_tag);
	static inline bool IsValid (uint8_t serialized_tag);
	static inline bool IsConst (uint8_t serialized_tag);
	static inline bool IsOddLength (uint8_t serialized_tag);
	static inline void RetagLengthOddness (STIR & st_ir);

	static inline STIR SerializeInt (uint32_t v);

	static inline STIR MakeBasic (bool is_const, Tag tag);
	static inline STIR MakeVariant (bool is_const, std::set<ID> const & allowed_types);
	static inline STIR MakeArray (bool is_const, Size size, ID type);
	static inline STIR MakeVector (bool is_const, ID type);
	static inline STIR MakeMap (bool is_const, ID key_type, ID value_type);
	static inline STIR MakeTuple (bool is_const, std::vector<ID> const & field_types);
	static inline STIR MakePackage (bool is_const, std::vector<ID> const & field_types);
	static inline STIR MakeFuction (bool is_const, ID return_type, std::vector<ID> const & param_types);

	static inline STIR MakeNil () {return MakeBasic(false, Tag::Nil);}
	static inline STIR MakeBool (bool is_const) {return MakeBasic(is_const, Tag::Bool);}
	static inline STIR MakeByte (bool is_const) {return MakeBasic(is_const, Tag::Byte);}
	static inline STIR MakeChar (bool is_const) {return MakeBasic(is_const, Tag::Char);}
	static inline STIR MakeInt (bool is_const) {return MakeBasic(is_const, Tag::Int);}
	static inline STIR MakeReal (bool is_const) {return MakeBasic(is_const, Tag::Real);}
	static inline STIR MakeString (bool is_const) {return MakeBasic(is_const, Tag::String);}
	static inline STIR MakeAny (bool is_const) {return MakeBasic(is_const, Tag::Any);}

	static inline int PackedSize (STIR const & st_ir);	// in bytes, rounded up
	static inline int STCode::UnpackedSize (PackedST const & packed_st);

	static inline PackedST Pack (STIR const & st_ir);
	static inline STIR Unpack (PackedST const & packed_st);

private:
	static inline uint8_t Qrtt (uint32_t v, int quartet);	// Zero is the low-order 4 bits
};

//----------------------------------------------------------------------
//======================================================================

class Registry
{
private:
	typedef uint32_t Entry;

public:
	Registry ();
	~Registry ();


private:

private:
	std::vector<Entry> m_types;
	std::basic_string<uint8_t> m_stash;

};

//======================================================================

	}	// namespace Types
}	// namespace UPL

//======================================================================
//======================================================================

namespace UPL {
	namespace Type {
	
//======================================================================

inline uint8_t STCode::SerializeTag (Tag tag, bool is_const, bool is_odd_length)
{
	uint8_t ret = TagToInt (tag);
	if (is_const)
		ret |= msc_ConstnessBit;
	if (is_odd_length)
		ret |= msc_OddLengthBit;
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

inline bool STCode::IsOddLength (uint8_t serialized_tag)
{
	return 0 != (serialized_tag & msc_OddLengthBit);
}

//----------------------------------------------------------------------

inline void STCode::RetagLengthOddness (STIR & st_ir)
{
	// Yeah! The length-oddness bit is set when the length is even! :D
	// But keep in mind that the first byte of STIR actually holds two quartets.
	if (st_ir.size() > 0 && (st_ir.size() % 2) == 0)
		st_ir[0] |= msc_OddLengthBit;
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

	return STIR({SerializeTag (tag, is_const, false)});
}

//----------------------------------------------------------------------

inline STIR STCode::MakeVariant (bool is_const, std::set<ID> const & allowed_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Variant, is_const, false);
	ret += SerializeInt (uint32_t(allowed_types.size()));
	for (auto id : allowed_types)
		ret += SerializeInt (id);
	RetagLengthOddness (ret);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeArray (bool is_const, Size size, ID type)
{
	STIR ret;

	ret += SerializeTag (Tag::Array, is_const, false);
	ret += SerializeInt (size);
	ret += SerializeInt (type);
	RetagLengthOddness (ret);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeVector (bool is_const, ID type)
{
	STIR ret;

	ret += SerializeTag (Tag::Vector, is_const, false);
	ret += SerializeInt (type);
	RetagLengthOddness (ret);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeMap (bool is_const, ID key_type, ID value_type)
{
	STIR ret;

	ret += SerializeTag (Tag::Map, is_const, false);
	ret += SerializeInt (key_type);
	ret += SerializeInt (value_type);
	RetagLengthOddness (ret);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeTuple (bool is_const, std::vector<ID> const & field_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Tuple, is_const, false);
	ret += SerializeInt (uint32_t(field_types.size()));
	for (auto id : field_types)
		ret += SerializeInt (id);
	RetagLengthOddness (ret);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakePackage (bool is_const, std::vector<ID> const & field_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Package, is_const, false);
	ret += SerializeInt (uint32_t(field_types.size()));
	for (auto id : field_types)
		ret += SerializeInt (id);
	RetagLengthOddness (ret);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeFuction (bool is_const, ID return_type, std::vector<ID> const & param_types)
{
	STIR ret;

	ret += SerializeTag (Tag::Tuple, is_const, false);
	ret += SerializeInt (return_type);
	ret += SerializeInt (uint32_t(param_types.size()));
	for (auto id : param_types)
		ret += SerializeInt (id);
	RetagLengthOddness (ret);

	return ret;
}

//----------------------------------------------------------------------

inline int STCode::PackedSize (STIR const & st_ir)
{
	assert (st_ir.size() > 0);
	// One byte for the first byte of STIR (which has 7 significant bits.)
	// Plus one byte per two bytes of STIR (each with 4 significant bits.)
	// The bottom bits of the last byte of output are filled with 0 if needed.
	return 1 + int(st_ir.size()) / 2;
}

//----------------------------------------------------------------------

inline int STCode::UnpackedSize (PackedST const & packed_st)
{
	assert (packed_st.size() > 0);
	return
		+ 1
		+ 2 * (int(packed_st.size()) - 1)
		- (IsOddLength(packed_st[0]) ? 1 : 0);
}

//----------------------------------------------------------------------

inline PackedST STCode::Pack (STIR const & st_ir)
{
	auto s = PackedSize(st_ir);
	
	assert (st_ir[st_ir.size()] == 0);	// Assume there's a NUL at the end of st_ir; this is BAD.

	PackedST ret (s, 0);

	ret[0] = st_ir[0];
	for (int i = 1, j = 1, n = int(st_ir.size()); j < n; ++i, j += 2)
		ret[i] = ((st_ir[j] & 0xF) << 4) | (st_ir[j + 1] & 0xF);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::Unpack (PackedST const & packed_st)
{
	auto s = UnpackedSize(packed_st);

	STIR ret (s, 0xFF);
	assert (ret[ret.size()] == 0);	// Assume there's a NUL at the end of ret; this is BAD.

	ret[0] = packed_st[0];
	for (int i = 1, j = 1, n = int(packed_st.size()); j < n; i += 2, ++j)
	{
		ret[i] = (packed_st[j] >> 4) & 0xF;
		ret[i + 1] = packed_st[j] & 0xF;
	}
	ret[ret.size()] = 0;

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