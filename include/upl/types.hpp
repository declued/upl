#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/definitions.hpp>
#include <upl/st_code.hpp>

#include <unordered_map>

//======================================================================

namespace UPL {
	namespace Type {

//======================================================================

class ScopedRegistry
{
private:
	typedef std::unordered_map<String, ID> NameLookup;

public:
	// Note: Registry does NOT own the STContainer
	explicit ScopedRegistry (STContainer * root_st_container, ScopedRegistry * parent_registry);
	~ScopedRegistry ();

	bool createName (String const & new_name, ID existing_type);
	ID findByName (String const & name) const;

private:
	STContainer * m_st_container = nullptr;
	ScopedRegistry * m_parent = nullptr;
	NameLookup m_names;
};

//----------------------------------------------------------------------
//======================================================================
//----------------------------------------------------------------------
//======================================================================

	}	// namespace Type
}	// namespace UPL

//======================================================================
