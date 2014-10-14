#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/definitions.hpp>

#include <memory>

//======================================================================

namespace UPL {
	namespace AST {

//======================================================================
/* These are the helper "Abstract Syntax Tree" node types. */
//----------------------------------------------------------------------

class Node
{
public:
	virtual ~Node () {}
	virtual Node * clone () const = 0;
	virtual String printable (int depth = 0) const = 0;

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
};

//----------------------------------------------------------------------

class Parent : public Node
{
public:
	typedef std::vector<std::unique_ptr<Node>> ChildContainer;

public:
	virtual ~Parent () {}
	virtual Node * clone () const = 0;

	ChildContainer & children () {return m_children;}
	ChildContainer const & children () const {return m_children;}

private:
	ChildContainer m_children;
};

//----------------------------------------------------------------------

class Value;

// I don't know what to do about the "type" of an expression exactly.
// For now, THIS IS JUST A PLACEHOLDER.
class Type {};

class ExprBase : public Parent
{
public:
	virtual ~ExprBase () {}
	virtual Node * clone () const = 0;

	virtual Type * evalType () const = 0;
	virtual Value * eval () const = 0;
};

//----------------------------------------------------------------------

class Value : public ExprBase
{
public:
	virtual ~Value () {}
	virtual Node * clone () const = 0;
};
//======================================================================
//----------------------------------------------------------------------
//======================================================================

	}	// namespace AST
}	// namespace UPL

//======================================================================

