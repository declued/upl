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

Registry::Registry ()
	: m_types ()
	, m_stash ()
	, m_lookup ()
{
	m_stash.reserve (10000);

	m_types.push_back ({}); m_types.back().bytes[3] = m_types.back().bytes[2] = m_types.back().bytes[1] = m_types.back().bytes[0] = 0;

	auto t1 = createType(STCode::Pack(STCode::MakeNil()));			assert (1 == t1);
	auto t2 = createType(STCode::Pack(STCode::MakeBool(false)));	assert (2 == t2);
	auto t3 = createType(STCode::Pack(STCode::MakeByte(false)));	assert (3 == t3);
	auto t4 = createType(STCode::Pack(STCode::MakeChar(false)));	assert (4 == t4);
	auto t5 = createType(STCode::Pack(STCode::MakeInt(false)));		assert (5 == t5);
	auto t6 = createType(STCode::Pack(STCode::MakeReal(false)));	assert (6 == t6);
	auto t7 = createType(STCode::Pack(STCode::MakeString(false)));	assert (7 == t7);
	auto t8 = createType(STCode::Pack(STCode::MakeAny(false)));		assert (8 == t8);
}

//----------------------------------------------------------------------

Registry::~Registry ()
{
}

//----------------------------------------------------------------------

ID Registry::createType (PackedST const & packed_st)
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
		for (int i = 0; i < packed_st.size() && i < 4; ++i)
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

ID Registry::lookupType (PackedST const & packed_st) const
{
	auto i = m_lookup.find(packed_st);
	if (m_lookup.end() != i)
		return i->second;
	else
		return InvalidID;
}

//----------------------------------------------------------------------

ID Registry::byTag (Tag tag) const
{
	auto const i = TagToInt(tag);
	if (i > 0 && i < TagToInt(Tag::Variant))
		return ID(i);
	else
		return InvalidID;
}

//----------------------------------------------------------------------

UnpackedType Registry::unpack (ID id) const
{
	return UnpackedType();
}

//======================================================================

	}	// namespace Details
}	// namespace UPL

//======================================================================
