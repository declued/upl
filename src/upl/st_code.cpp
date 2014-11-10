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

	}	// namespace Details
}	// namespace UPL

//======================================================================
