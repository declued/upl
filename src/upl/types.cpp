//======================================================================

#include <upl/types.hpp>

//======================================================================

namespace UPL {
	namespace Type {

//======================================================================

ScopedRegistry::ScopedRegistry (STContainer * root_st_container, ScopedRegistry * parent_registry)
	: m_st_container (root_st_container)
	, m_parent (parent_registry)
	, m_names ()
{
	assert (nullptr != m_st_container);
}

//----------------------------------------------------------------------

ScopedRegistry::~ScopedRegistry ()
{
}

//----------------------------------------------------------------------

bool ScopedRegistry::createName (String const & new_name, ID existing_type)
{
	if (!m_st_container->isValid(existing_type))
		return false;
	if (m_names.find(new_name) != m_names.end())
		return false;

	m_names[new_name] = existing_type;

	return true;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//======================================================================

	}	// namespace Type
}	// namespace UPL

//======================================================================
