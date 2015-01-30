//======================================================================

#include <upl/st_code.hpp>

//======================================================================

namespace UPL {
	namespace Type {
	
//======================================================================

#define TAG_INFO_STRUCT(v,e,s,b0,b1,b2,b3,b4)	{Tag::e, s, b0, b1, b2, b3, b4},
static TagInfo_t const gsc_TagInfo [TagCount] = { UPL_PRIVATE__TYPE_TAGS(TAG_INFO_STRUCT) };
#undef  TAG_INFO_STRUCT

#define TAG_CHECK_VALUE(v,e,s,b0,b1,b2,b3,b4)	static_assert (int(gsc_TagInfo[v].tag) == v, "");
// :((
// Why can't I do this?!
//UPL_PRIVATE__TYPE_TAGS(TAG_CHECK_VALUE)
#undef  TAG_CHECK_VALUE

//----------------------------------------------------------------------

TagInfo_t TagInfo (Tag tag)
{
	assert (TagToInt(tag) < TagCount);

	return gsc_TagInfo[TagToInt(tag)];
}

//======================================================================

PackedST Unpacked::pack () const
{
	switch (tag)
	{
	case Tag::INVALID:	return {};
	case Tag::Nil:		return STCode::Pack(STCode::MakeNil());
	case Tag::Bool:		return STCode::Pack(STCode::MakeBool(is_const));
	case Tag::Byte:		return STCode::Pack(STCode::MakeByte(is_const));
	case Tag::Char:		return STCode::Pack(STCode::MakeChar(is_const));
	case Tag::Int:		return STCode::Pack(STCode::MakeInt(is_const));
	case Tag::Real:		return STCode::Pack(STCode::MakeReal(is_const));
	case Tag::String:	return STCode::Pack(STCode::MakeString(is_const));
	case Tag::Any:		return STCode::Pack(STCode::MakeAny(is_const));
	case Tag::Variant:	return STCode::Pack(STCode::MakeVariant(is_const, {type_list.begin(), type_list.end()}));
	case Tag::Array:	return STCode::Pack(STCode::MakeArray(is_const, size, type1));
	case Tag::Vector:	return STCode::Pack(STCode::MakeVector(is_const, type1));
	case Tag::Map:		return STCode::Pack(STCode::MakeMap(is_const, type1, type2));
	case Tag::Tuple:	return STCode::Pack(STCode::MakeTuple(is_const, type_list));
	case Tag::Package:	return STCode::Pack(STCode::MakePackage(is_const, type_list));
	case Tag::Function:	return STCode::Pack(STCode::MakeFuction(is_const, type1, type_list));
	default:			assert(false); return {};
	}
}

//======================================================================

STContainer::STContainer ()
	: m_types ()
	, m_stash ()
	, m_lookup ()
{
	m_stash.reserve (10000);

	// Make the invalid entry
	m_types.push_back ({});
	m_types.back().bytes[0] = 0;
	m_types.back().bytes[1] = 0;
	m_types.back().bytes[2] = 0;
	m_types.back().bytes[3] = 0;

	// Make the rest of the default entries
	auto t01 = createType(STCode::Pack(STCode::MakeNil()));			assert ( 1 == t01);
	auto t02 = createType(STCode::Pack(STCode::MakeBool(false)));	assert ( 2 == t02);
	auto t03 = createType(STCode::Pack(STCode::MakeBool(true)));	assert ( 3 == t03);
	auto t04 = createType(STCode::Pack(STCode::MakeByte(false)));	assert ( 4 == t04);
	auto t05 = createType(STCode::Pack(STCode::MakeByte(true)));	assert ( 5 == t05);
	auto t06 = createType(STCode::Pack(STCode::MakeChar(false)));	assert ( 6 == t06);
	auto t07 = createType(STCode::Pack(STCode::MakeChar(true)));	assert ( 7 == t07);
	auto t08 = createType(STCode::Pack(STCode::MakeInt(false)));	assert ( 8 == t08);
	auto t09 = createType(STCode::Pack(STCode::MakeInt(true)));		assert ( 9 == t09);
	auto t10 = createType(STCode::Pack(STCode::MakeReal(false)));	assert (10 == t10);
	auto t11 = createType(STCode::Pack(STCode::MakeReal(true)));	assert (11 == t11);
	auto t12 = createType(STCode::Pack(STCode::MakeString(false)));	assert (12 == t12);
	auto t13 = createType(STCode::Pack(STCode::MakeString(true)));	assert (13 == t13);
	auto t14 = createType(STCode::Pack(STCode::MakeAny(false)));	assert (14 == t14);
	auto t15 = createType(STCode::Pack(STCode::MakeAny(true)));		assert (15 == t15);
}

//----------------------------------------------------------------------

STContainer::~STContainer ()
{
}

//----------------------------------------------------------------------

ID STContainer::createType (PackedST const & packed_st)
{
	if (!STCode::IsValid(packed_st))
		return InvalidID;

	auto id = lookupType(packed_st);
	if (InvalidID != id)
		return id;

	m_types.push_back ({});

	ID cur_id = ID(m_types.size()) - 1;
	auto & cur = m_types.back();

	m_lookup[packed_st] = cur_id;

	if (packed_st.size() <= sizeof(Entry))	// Put it in-line
	{
		cur.bytes[3] = cur.bytes[2] = cur.bytes[1] = cur.bytes[0] = 0;
		for (size_t i = 0; i < packed_st.size() && i < 4; ++i)
			cur.bytes[i] = packed_st[i];
		setInlineEntry (cur_id, true);
	}
	else									// Put it in the stash
	{
		auto stash_index = stashCurPos();
		m_stash += packed_st;
		assert (m_stash.size() == stash_index + packed_st.size());
		setStashIndex (cur_id, stash_index);
	}
	
	return cur_id;	
}

//----------------------------------------------------------------------

ID STContainer::createType (Unpacked const & unpacked)
{
	return createType(unpacked.pack());
}

//----------------------------------------------------------------------

ID STContainer::lookupType (PackedST const & packed_st) const
{
	auto i = m_lookup.find(packed_st);
	if (m_lookup.end() != i)
		return i->second;
	else
		return InvalidID;
}

//----------------------------------------------------------------------

ID STContainer::byTag (Tag tag) const
{
	auto const i = TagToInt(tag);
	if (i > 0 && i < TagToInt(Tag::Variant))
		return ID(i);
	else
		return InvalidID;
}

//----------------------------------------------------------------------

Unpacked STContainer::unpack (ID id) const
{
	auto t = tag(id);
	auto c = isConst(id);

	Unpacked ret (t, c);

	switch (t)
	{
	case Tag::Variant:
		ret.type_list = getVariantTypes(id);
		break;
	case Tag::Array:
		ret.size = getArraySize(id);
		ret.type1 = getArrayType(id);
		break;
	case Tag::Vector:
		ret.type1 = getVectorType(id);
		break;
	case Tag::Map:
		ret.type1 = getMapKeyType(id);
		ret.type2 = getMapValueType(id);
		break;
	case Tag::Tuple:
		ret.type_list = getTupleTypes(id);
		break;
	case Tag::Package:
		ret.type_list = getPackageTypes(id);
		break;
	case Tag::Function:
		ret.type1 = getFunctionReturnType(id);
		ret.type_list = getFunctionParamTypes(id);
		break;
	case Tag::Nil:
		/* TODO */
		break;
	case Tag::Bool:
		/* TODO */
		break;
	case Tag::Byte:
		/* TODO */
		break;
	case Tag::Char:
		/* TODO */
		break;
	case Tag::Int:
		/* TODO */
		break;
	case Tag::Real:
		/* TODO */
		break;
	case Tag::String:
		/* TODO */
		break;
	case Tag::Any:
		/* TODO */
		break;
	case Tag::INVALID:
		/* TODO */
		break;
	};

	return ret;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

std::vector<ID> STContainer::getTypeList (ID id) const
{
	assert (tag(id) == Tag::Variant || tag(id) == Tag::Tuple || tag(id) == Tag::Package);
	auto qfetch = [this, id](int q){return this->getQuartet(id, q);};
	int q = 0;

	size_t cnt = STCode::DeserializeInt (qfetch, q);
	std::vector<ID> ret (cnt);
	for (size_t i = 0; i < cnt; ++i)
		ret[i] = STCode::DeserializeInt (qfetch, q);
	return ret;
}

//----------------------------------------------------------------------

std::vector<ID> STContainer::getParamTypeList (ID id) const
{
	assert (tag(id) == Tag::Function);
	auto qfetch = [this, id](int q){return this->getQuartet(id, q);};
	int q = 0;

	STCode::DeserializeInt (qfetch, q);

	size_t cnt = STCode::DeserializeInt (qfetch, q);
	std::vector<ID> ret (cnt);
	for (size_t i = 0; i < cnt; ++i)
		ret[i] = STCode::DeserializeInt (qfetch, q);
	return ret;
}

//----------------------------------------------------------------------

ID STContainer::getFirstType (ID id) const
{
	assert (tag(id) == Tag::Vector || tag(id) == Tag::Map || tag(id) == Tag::Function);
	auto qfetch = [this, id](int q){return this->getQuartet(id, q);};
	int q = 0;

	return STCode::DeserializeInt (qfetch, q);
}

//----------------------------------------------------------------------

ID STContainer::getSecondType (ID id) const
{
	assert (tag(id) == Tag::Array || tag(id) == Tag::Map);
	auto qfetch = [this, id](int q){return this->getQuartet(id, q);};
	int q = 0;

	STCode::DeserializeInt (qfetch, q);

	return STCode::DeserializeInt (qfetch, q);
}

//----------------------------------------------------------------------

Size STContainer::getSize (ID id) const
{
	assert (tag(id) == Tag::Array);
	auto qfetch = [this, id](int q){return this->getQuartet(id, q);};
	int q = 0;

	return STCode::DeserializeInt (qfetch, q);
}

//----------------------------------------------------------------------
//======================================================================

	}	// namespace Type
}	// namespace UPL

//======================================================================
