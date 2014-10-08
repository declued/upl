//======================================================================

#include <upl/common.hpp>

#include <sstream>

//======================================================================

namespace UPL {

//======================================================================

Char InvalidChar ()
{
	return 0;
}

//----------------------------------------------------------------------

bool IsInvalid (Char c)
{
	return InvalidChar() == c;
}

//----------------------------------------------------------------------

bool IsNewline (Char c)
{
	return '\n' == c;
}

//----------------------------------------------------------------------

bool IsWhitespace (Char c)
{
	return ' ' == c || '\n' == c || '\r' == c || '\t' == c || '\f' == c || '\v' == c;
}

//----------------------------------------------------------------------

bool IsDigit (Char c)
{
	return '0' <= c && c <= '9';
}

//----------------------------------------------------------------------

bool IsLetter (Char c)
{
	return IsLower(c) || IsUpper(c);
}

//----------------------------------------------------------------------

bool IsLower (Char c)
{
	return 'a' <= c && c <= 'z';
}

//----------------------------------------------------------------------

bool IsUpper (Char c)
{
	return 'A' <= c && c <= 'Z';
}

//----------------------------------------------------------------------

bool IsAlphanum (Char c)
{
	return IsLetter(c) || IsDigit(c);
}

//----------------------------------------------------------------------

bool IsIdentStarter (Char c)
{
	return IsLetter(c) /*|| (c == '_')*/;
}

//----------------------------------------------------------------------

bool IsIdentContinuer (Char c)
{
	return IsAlphanum(c) || (c == '_');
}

//----------------------------------------------------------------------
//======================================================================

template <typename T>
String ToString (T v)
{
	return std::to_wstring(std::move(v));
}

//----------------------------------------------------------------------

template <>
String ToString<std::string> (std::string str)
{
	String ret;
	ret.reserve (str.size());
	for (auto c : str)
		ret += Char(c);
	return ret;
}

//----------------------------------------------------------------------

template <>
String ToString<char const *> (char const * str)
{
	String ret;
	for (int i = 0; str[i]; ++i)
		ret += Char(str[i]);
	return ret;
}

//----------------------------------------------------------------------

template <>
String ToString<wchar_t const *> (wchar_t const * str)
{
	String ret;
	for (int i = 0; str[i]; ++i)
		ret += Char(str[i]);
	return ret;
}

//----------------------------------------------------------------------

template <>
String ToString<std::wstring> (std::wstring str)
{
	return std::move(str);
}

//----------------------------------------------------------------------

template <>
String ToString<bool> (bool v)
{
	return v ? L"true" : L"false";
}

//----------------------------------------------------------------------

/* Now the explicit instantiations for the above: */
template String ToString<std::string> (std::string);
template String ToString<std::wstring> (std::wstring);
template String ToString<char const *> (char const *);
template String ToString<wchar_t const *> (wchar_t const *);
template String ToString<  int8_t> ( int8_t);
template String ToString< uint8_t> (uint8_t);
template String ToString< int16_t> ( int16_t);
template String ToString<uint16_t> (uint16_t);
template String ToString< int32_t> ( int32_t);
template String ToString<uint32_t> (uint32_t);
template String ToString< int64_t> ( int64_t);
template String ToString<uint64_t> (uint64_t);
template String ToString<float> (float);
template String ToString<double> (double);
template String ToString<bool> (bool);

//----------------------------------------------------------------------
//----------------------------------------------------------------------

template <typename T>
T FromString (String const & str)
{
	std::wstringstream wss (str);
	T ret;
	wss >> ret;
	return ret;
}

//----------------------------------------------------------------------

template <>
std::string FromString<std::string> (String const & str)
{
	std::string ret;
	ret.reserve (str.size());
	for (auto c : str)
		if (c > 255)
			ret += '.';
		else
			ret += char(c);
	return ret;
}

//----------------------------------------------------------------------

template <>
std::wstring FromString<std::wstring> (String const & str)
{
	return str;
}

//----------------------------------------------------------------------

/* Now the explicit instantiations for the above: */
template std::string FromString<std::string> (String const &);
template std::wstring FromString<std::wstring> (String const &);
//template int8_t FromString<int8_t> (String const &);
//template uint8_t FromString<uint8_t> (String const &);
template int16_t FromString<int16_t> (String const &);
template uint16_t FromString<uint16_t> (String const &);
template int32_t FromString<int32_t> (String const &);
template uint32_t FromString<uint32_t> (String const &);
template int64_t FromString<int64_t> (String const &);
template uint64_t FromString<uint64_t> (String const &);
template float FromString<float> (String const &);
template double FromString<double> (String const &);
//template bool FromString<bool> (String const &);

//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
