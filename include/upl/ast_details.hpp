#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/definitions.hpp>
#include <upl/types.hpp>

#include <memory>

//======================================================================

namespace UPL {
	namespace AST {

//======================================================================

template <typename T>
using Ptr = T *;

//======================================================================
/* These are the helper node types for the Abstract Syntax Tree impl. */
//----------------------------------------------------------------------

class Node
{
public:
	virtual ~Node () {}
	//virtual Node * clone () const = 0;
	//virtual String printable (int depth = 0) const = 0;

	/* more convenient casting */
	template <typename T>
	T * as ()
	{
		auto ret = dynamic_cast<T *>(this);
		assert (nullptr != ret);
		return ret;
	}

	template <typename T>
	T const * as () const
	{
		auto ret = dynamic_cast<T const *>(this);
		assert (nullptr != ret);
		return ret;
	}

	template <typename T>
	bool canBe () const
	{
		return nullptr != dynamic_cast<T const *>(this);
	}
};

//----------------------------------------------------------------------

class Parent : public Node
{
public:
	typedef std::vector<std::unique_ptr<Node>> ChildContainer;

public:
	ChildContainer & children () {return m_children;}
	ChildContainer const & children () const {return m_children;}
	void addChild (Ptr<Node> new_child) {if (new_child) m_children.emplace_back(std::move(new_child));}

private:
	ChildContainer m_children;
};

//----------------------------------------------------------------------

class Value;

//----------------------------------------------------------------------

class ExprBase : public Node
{
public:
	virtual Type::ID type () const = 0;
	virtual Value * eval () const = 0;
};

//----------------------------------------------------------------------

class ExprBaseParent : public Parent, public ExprBase
{
public:
};

//----------------------------------------------------------------------

class Value : public ExprBase
{
public:
};
//======================================================================
//----------------------------------------------------------------------
//======================================================================

	}	// namespace AST
}	// namespace UPL

//======================================================================

