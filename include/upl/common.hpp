#pragma once

//======================================================================

#if defined(_MSC_VER)
	#define _CRT_SECURE_NO_WARNINGS
#endif

//======================================================================

/* Everybody needs these: */
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

//======================================================================

#if defined(_MSC_VER)
	#define UPL_NOEXCEPT	throw()
	#define vsnprintf		_vsnprintf
#else
	#define UPL_NOEXCEPT	nothrow
#endif

//======================================================================

#define UPL_STRINGIZE_IMPL(s)	#s
#define UPL_STRINGIZE(s)		UPL_STRINGIZE_IMPL(s)

//======================================================================

namespace UPL {

//======================================================================

typedef wchar_t Char;
typedef std::wstring String;

typedef int64_t Int;
typedef double Real;
typedef bool Bool;

typedef std::string Path;

//======================================================================

/* Character Classification: */
Char InvalidChar ();
bool IsInvalid (Char c);
bool IsNewline (Char c);
bool IsWhitespace (Char c);
bool IsDigit (Char c);
bool IsLetter (Char c);
bool IsLower (Char c);
bool IsUpper (Char c);
bool IsAlphanum (Char c);
bool IsIdentStarter (Char c);
bool IsIdentContinuer (Char c);

//======================================================================

class Location
{
public:
	Location (int line_no_ = 1, int column_no_ = 0, int char_no_ = 0)
		: m_line_no (line_no_), m_column_no (column_no_), m_char_no (char_no_)
	{}

	int line () const {return m_line_no;}
	int column () const {return m_column_no;}
	int totalChars () const {return m_char_no;}

	void feed (Char c)
	{
		m_char_no += 1;
		if (IsNewline(c))
			m_line_no += 1, m_column_no = 0;
		else
			m_column_no += 1;
	}

private:
	int m_line_no;
	int m_column_no;
	int m_char_no;
};

//======================================================================

/* String utilities: */
template <typename T>
String ToString (T v);

template <typename T>
T FromString (String const & str);

//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
