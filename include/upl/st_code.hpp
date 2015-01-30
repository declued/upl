#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/definitions.hpp>

#include <set>
#include <unordered_map>
#include <utility>

//======================================================================

#ifdef __GNUG__

/*
 * GCC seems to not have hash<basic_string<uint8_t> >. We provide a simple
 * implementation as a workaround.
 */
namespace std {
	template <> struct hash<basic_string<uint8_t> >
	{
		inline size_t operator()(const basic_string<uint8_t> & us) const
		{
			hash<char *> cstring_hash;
			return cstring_hash((char *) us.c_str());
		}
	};
}

#endif /* __GNUG__ */

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

ID const InvalidID = 0;

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

struct Unpacked
{
	Tag tag = Tag::INVALID;
	ID type1 = InvalidID;
	ID type2 = InvalidID;
	Size size = 0;
	std::vector<ID> type_list;
	bool is_const = false;

	Unpacked () {}
	explicit Unpacked (Tag tag_, bool is_const_) : tag (tag_), is_const (is_const_) {}
	Unpacked (Tag tag_, bool is_const_, ID type1_) : tag (tag_), type1 (type1_), is_const (is_const_) {}
	Unpacked (Tag tag_, bool is_const_, ID type1_, ID type2_, Size size_) : tag (tag_), type1 (type1_), type2 (type2_), size (size_), is_const (is_const_) {}
	Unpacked (Tag tag_, bool is_const_, std::vector<ID> type_list_) : tag (tag_), type_list (std::move(type_list_)), is_const (is_const_) {}
	Unpacked (Tag tag_, bool is_const_, ID type1_, std::vector<ID> type_list_) : tag (tag_), type1 (type1_), type_list (std::move(type_list_)), is_const (is_const_) {}

	PackedST pack () const;
};

//======================================================================

class STCode
{
public:
	static uint8_t const msc_StashedEntryBit = (1 << 7);
	static uint8_t const msc_ConstnessBit = (1 << 6);
	static uint8_t const msc_OddLengthBit = (1 << 5);

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
	static inline uint32_t DeserializeInt (uint8_t const * const mem, unsigned & start_qrtt);
	
	// Returns the deserialized number and the number of quartets it required
	template <typename F>
	static inline std::pair<uint32_t, int> DeserializeInt (int base_quartet, F const & quartet_fetcher);
	template <typename F>
	static inline uint32_t DeserializeInt (F const & quartet_fetcher, int & start_quartet);

	static inline STIR MakeInvalid ();
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
	static inline int UnpackedSize (PackedST const & packed_st);

	static inline PackedST Pack (STIR const & st_ir);
	static inline STIR Unpack (PackedST const & packed_st);

	static inline bool IsValid (PackedST const & packed_st);

private:
	static inline uint8_t Qrtt (uint32_t v, unsigned quartet);	// Zero is the low-order 4 bits
	static inline uint8_t Qrtt (uint8_t const * packed_mem, unsigned quartet);	// Zero is the high-order 4 bits
};

//----------------------------------------------------------------------
//======================================================================

class STContainer
{
private:
	struct Entry { uint8_t bytes [4]; };
	typedef std::unordered_map<PackedST, ID> TypeLookup;

public:
	STContainer ();
	~STContainer ();

	size_t size () const {return m_types.size();}

	ID createType (PackedST const & packed_st);
	ID createType (Unpacked const & unpacked);

	ID lookupType (PackedST const & packed_st) const;
	ID byTag (Tag tag) const;

	inline bool isValid (ID id) const;
	inline Tag tag (ID id) const;
	inline bool isConst (ID id) const;

	inline std::vector<ID> getVariantTypes (ID id) const;
	inline std::vector<ID> getTupleTypes (ID id) const;
	inline std::vector<ID> getPackageTypes (ID id) const;
	inline std::vector<ID> getFunctionParamTypes (ID id) const;
	inline ID getArrayType (ID id) const;
	inline ID getVectorType (ID id) const;
	inline ID getMapKeyType (ID id) const;
	inline ID getMapValueType (ID id) const;
	inline ID getFunctionReturnType (ID id) const;
	inline Size getArraySize (ID id) const;

	Unpacked unpack (ID id) const;

private:
	uint32_t stashCurPos() const { return uint32_t(m_stash.size()); }
	inline bool isInline (ID id) const;
	inline uint32_t stashedIndex (ID id) const;
	inline void setInlineEntry (ID id, bool is_inline);
	inline void setStashIndex (ID id, uint32_t stash_index);
	inline uint8_t getByte (ID id, int b) const;
	inline uint8_t getQuartet (ID id, int q) const;	// Starting from _after_ the first byte (the tag byte.)

	std::vector<ID> getTypeList (ID id) const;		// For Variant, Tuple, Package
	std::vector<ID> getParamTypeList (ID id) const;	// For Function
	ID getFirstType (ID id) const;					// For Array, Vector, Map (key), Function (return type)
	ID getSecondType (ID id) const;					// For Map (value)
	Size getSize (ID id) const;						// For Array

private:
	std::vector<Entry> m_types;
	std::basic_string<uint8_t> m_stash;
	TypeLookup m_lookup;

private:
	static_assert (sizeof(Entry) == 4, "Entry was expected to be 4 bytes long.");
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
	static_assert (msc_OddLengthBit < msc_ConstnessBit, "");

	return IntToTag (serialized_tag & (msc_OddLengthBit - 1));
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

inline uint32_t STCode::DeserializeInt (uint8_t const * const packed_mem, unsigned & q)
{
	auto len = Qrtt(packed_mem, q++);

	if (len < 8) return len;
	
	uint32_t ret = 0;
	for (int i = len - 7; i > 0; --i)
		ret = (ret << 4) | Qrtt(packed_mem, q++);

	return ret;
}

//----------------------------------------------------------------------

template <typename F>
inline std::pair<uint32_t, int> STCode::DeserializeInt (int base_quartet, F const & quartet_fetcher)
{
	uint8_t len = quartet_fetcher(base_quartet + 0);
	if (len < 8) return {len, 1};
	
	len -= 6;
	uint32_t ret = 0;
	for (int i = 1; i < len; ++i)
		ret = (ret << 4) | quartet_fetcher(base_quartet + i);

	return {ret, len};
}

//----------------------------------------------------------------------

template <typename F>
inline uint32_t STCode::DeserializeInt (F const & quartet_fetcher, int & start_quartet)
{
	auto len = quartet_fetcher (start_quartet++);
	if (len < 8) return len;
	
	uint32_t ret = 0;
	for (int i = len - 7; i > 0; --i)
		ret = (ret << 4) | quartet_fetcher(start_quartet++);

	return ret;
}

//----------------------------------------------------------------------

inline STIR STCode::MakeInvalid ()
{
	return STIR({SerializeTag (Tag::INVALID, false, false)});
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

	ret += SerializeTag (Tag::Function, is_const, false);
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

inline bool STCode::IsValid (PackedST const & packed_st)
{
	return packed_st.size() > 0 && IsValid(packed_st[0]);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

inline uint8_t STCode::Qrtt (uint32_t v, unsigned quartet)
{
	return (v >> (4 * quartet)) & 0x0F;
}

//----------------------------------------------------------------------

inline uint8_t STCode::Qrtt (uint8_t const * packed_mem, unsigned quartet)
{
	unsigned const shift = 4 * (1 - (quartet & 1));
	return (packed_mem[quartet >> 1] >> shift) & 0x0F;
}

//======================================================================

inline bool STContainer::isValid (ID id) const
{
	return id < m_types.size();
}

//----------------------------------------------------------------------

inline Tag STContainer::tag (ID id) const
{
	if (id >= m_types.size())
		return Tag::INVALID;
	else
		return STCode::GetTag(getByte(id, 0));
}

//----------------------------------------------------------------------

inline bool STContainer::isConst (ID id) const
{
	if (id >= m_types.size())
		return false;
	else
		return STCode::IsConst(getByte(id, 0));
}

//----------------------------------------------------------------------

inline std::vector<ID> STContainer::getVariantTypes (ID id) const
{
	return getTypeList(id);
}

//----------------------------------------------------------------------

inline std::vector<ID> STContainer::getTupleTypes (ID id) const
{
	return getTypeList(id);
}

//----------------------------------------------------------------------

inline std::vector<ID> STContainer::getPackageTypes (ID id) const
{
	return getTypeList(id);
}

//----------------------------------------------------------------------

inline std::vector<ID> STContainer::getFunctionParamTypes (ID id) const
{
	return getParamTypeList(id);
}

//----------------------------------------------------------------------

inline ID STContainer::getArrayType (ID id) const
{
	return getSecondType(id);
}

//----------------------------------------------------------------------

inline ID STContainer::getVectorType (ID id) const
{
	return getFirstType(id);
}

//----------------------------------------------------------------------

inline ID STContainer::getMapKeyType (ID id) const
{
	return getFirstType(id);
}

//----------------------------------------------------------------------

inline ID STContainer::getMapValueType (ID id) const
{
	return getSecondType(id);
}

//----------------------------------------------------------------------

inline ID STContainer::getFunctionReturnType (ID id) const
{
	return getFirstType(id);
}

//----------------------------------------------------------------------

inline Size STContainer::getArraySize (ID id) const
{
	return getSize(id);
}

//----------------------------------------------------------------------

inline bool STContainer::isInline (ID id) const
{
	return 0 == (m_types[id].bytes[0] & STCode::msc_StashedEntryBit);
}

//----------------------------------------------------------------------

inline uint32_t STContainer::stashedIndex (ID id) const
{
	assert(!isInline(id));
	auto b = m_types[id];
	return ((b.bytes[0] & 0x7F) << 24) | (b.bytes[1] << 16) | (b.bytes[2] << 8) | b.bytes[3];
}

//----------------------------------------------------------------------

inline void STContainer::setInlineEntry (ID id, bool is_inline)
{
	if (!is_inline)
		m_types[id].bytes[0] |= STCode::msc_StashedEntryBit;
	else
	{
		assert (0 == (m_types[id].bytes[0] & STCode::msc_StashedEntryBit));
	}
}

//----------------------------------------------------------------------

inline void STContainer::setStashIndex (ID id, uint32_t stash_index)
{
	assert (stash_index < 0x80000000U);	// Must keep the msb free.
	m_types[id].bytes[0] = (stash_index >> 24) & 0x7F;
	m_types[id].bytes[1] = (stash_index >> 16) & 0xFF;
	m_types[id].bytes[2] = (stash_index >>  8) & 0xFF;
	m_types[id].bytes[3] = stash_index & 0xFF;
	setInlineEntry (id, false);
	assert (!isInline(id));
	assert (stashedIndex(id) == stash_index);
}

//----------------------------------------------------------------------

inline uint8_t STContainer::getByte (ID id, int b) const
{
	return isInline(id) ? m_types[id].bytes[b] : m_stash[stashedIndex(id) + b];
}

//----------------------------------------------------------------------

inline uint8_t STContainer::getQuartet (ID id, int q) const
{
	auto b = getByte(id, 1 + q / 2);
	return 0xF & ((q & 1) ? b : (b >> 4));
}

//----------------------------------------------------------------------
//======================================================================

	}	// namespace Types
}	// namespace UPL

//======================================================================
