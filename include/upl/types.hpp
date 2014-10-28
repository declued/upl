#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/definitions.hpp>

#include <memory>				// for shared_ptr
#include <unordered_map>

//======================================================================

namespace UPL {
	namespace Types {

//======================================================================

enum class Tag : uint8_t
{
	Invalid,

	Null,

	Bool,
	Byte,
	Char,
	Int,
	Real,

	Any,

	String,
	Array,
	Table,

	Function,

	Package,
};

//----------------------------------------------------------------------
//======================================================================

class Type;
typedef std::shared_ptr<Type> TypePtr;

typedef std::string SType;

//----------------------------------------------------------------------

class Type
{
public:
	static TypePtr Deserialize (SType const & s);

public:
	explicit Type (Tag tag, String name) : m_name (std::move(name)), m_tag (tag) {}
	virtual ~Type () {}

	virtual bool is (TypePtr const that) const = 0;
	virtual bool is (Type const & that) const = 0;
	virtual size_t sizeOf () const = 0;
	virtual bool basic () const = 0;
	virtual bool scalar () const = 0;
	virtual bool composite () const = 0;
	virtual bool collection () const = 0;
	virtual SType serialize () const = 0;

	Tag tag () const {return m_tag;}
	String const & name () const {return m_name;}

	bool operator == (Type const & that) const {return is(that);}

	template <typename T> T * as () {return dynamic_cast<T *>(this);}
	template <typename T> T const * as () const {return dynamic_cast<T const *>(this);}

	template <typename T> T * unsafeAs () {return static_cast<T *>(this);}
	template <typename T> T const * unsafeAs () const {return static_cast<T const *>(this);}

private:
	String m_name;
	Tag m_tag;
};

//======================================================================

class Null : public Type
{
public:
	Null () : Type (Tag::Null, L"null") {}

	virtual bool is (TypePtr const that) const override { return tag() == that->tag(); }
	virtual bool is (Type const & that) const override { return tag() == that.tag(); }
	virtual size_t sizeOf () const override { return 0; }
	virtual bool basic () const override { return true; }
	virtual bool scalar () const override { return true; }
	virtual bool composite () const override { return false; }
	virtual bool collection () const override { return false; }
};

//----------------------------------------------------------------------

class Bool : public Type
{
public:
	Bool () : Type (Tag::Bool, L"bool") {}

	virtual bool is (TypePtr const that) const override { return tag() == that->tag(); }
	virtual bool is (Type const & that) const override { return tag() == that.tag(); }
	virtual size_t sizeOf () const override { return 1; }
	virtual bool basic () const override { return true; }
	virtual bool scalar () const override { return true; }
	virtual bool composite () const override { return false; }
	virtual bool collection () const override { return false; }
};

//----------------------------------------------------------------------

class Byte : public Type
{
public:
	Byte () : Type (Tag::Byte, L"byte") {}

	virtual bool is (TypePtr const that) const override { return tag() == that->tag(); }
	virtual bool is (Type const & that) const override { return tag() == that.tag(); }
	virtual size_t sizeOf () const override { return 1; }
	virtual bool basic () const override { return true; }
	virtual bool scalar () const override { return true; }
	virtual bool composite () const override { return false; }
	virtual bool collection () const override { return false; }
};

//----------------------------------------------------------------------

class Char : public Type
{
public:
	Char () : Type (Tag::Char, L"char") {}

	virtual bool is (TypePtr const that) const override { return tag() == that->tag(); }
	virtual bool is (Type const & that) const override { return tag() == that.tag(); }
	virtual size_t sizeOf () const override { return 2; }
	virtual bool basic () const override { return true; }
	virtual bool scalar () const override { return true; }
	virtual bool composite () const override { return false; }
	virtual bool collection () const override { return false; }
};

//----------------------------------------------------------------------

class Int : public Type
{
public:
	Int () : Type (Tag::Int, L"int") {}

	virtual bool is (TypePtr const that) const override { return tag() == that->tag(); }
	virtual bool is (Type const & that) const override { return tag() == that.tag(); }
	virtual size_t sizeOf () const override { return 8; }
	virtual bool basic () const override { return true; }
	virtual bool scalar () const override { return true; }
	virtual bool composite () const override { return false; }
	virtual bool collection () const override { return false; }
};

//----------------------------------------------------------------------

class Real : public Type
{
public:
	Real () : Type (Tag::Real, L"real") {}

	virtual bool is (TypePtr const that) const override { return tag() == that->tag(); }
	virtual bool is (Type const & that) const override { return tag() == that.tag(); }
	virtual size_t sizeOf () const override { return 8; }
	virtual bool basic () const override { return true; }
	virtual bool scalar () const override { return true; }
	virtual bool composite () const override { return false; }
	virtual bool collection () const override { return false; }
};

//----------------------------------------------------------------------

class Function : public Type
{
public:
	Function (TypePtr return_type, std::vector<TypePtr> param_types)
		: Type (Tag::Function, L"function")
		, m_param_types (std::move(param_types))
		, m_return_type (std::move(return_type))
	{}

	virtual bool is (TypePtr const that) const override {return is(*that);}
	virtual bool is (Type const & that) const override
	{
		return 
			tag() == that.tag() &&
			returnsTheSameAs(*that.unsafeAs<Function>()) &&
			takesTheSameAs(*that.unsafeAs<Function>())
		;
	}

	virtual size_t sizeOf () const override {return UPL_PRIVATE__POINTER_SIZE;}
	virtual bool basic () const override {return false;}
	virtual bool scalar () const override {return true;}
	virtual bool composite () const override {return false;}
	virtual bool collection () const override {return false;}

	int paramCount () const {return int(m_param_types.size());}
	TypePtr paramType (int idx) const {
		return (idx < 0 || idx >= int(m_param_types.size()))
			? nullptr
			: m_param_types[idx]
		;
	}

	TypePtr retType () const {return m_return_type;}

	bool returnsTheSameAs (Function const & that) const {return retType()->is(that.retType());}
	bool takesTheSameAs (Function const & that) const {
		if (paramCount() != that.paramCount()) return false;
		for (int i = 0, n = paramCount(); i < n; ++i)
			if (!(paramType(i)->is(that.paramType(i))))
				return false;
		return true;
	}

private:
	std::vector<TypePtr> m_param_types;
	TypePtr m_return_type;
};

//======================================================================

class Registry
{
private:
	typedef std::unordered_map<String, TypePtr> Container;

public:
	explicit Registry (Registry * parent);
	~Registry ();

	// Returns true if type is indeed new
	bool registerType (std::unique_ptr<Type> new_type);
	TypePtr findByName (String const & name) const;

private:
	Registry * m_parent;
	Container m_types;
};

//======================================================================

	}	// namespace Types
}	// namespace UPL

//======================================================================
