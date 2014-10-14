//======================================================================

#include <upl/input.hpp>
#include <cstdio>

//======================================================================

namespace UPL {

//======================================================================

UTF8FileStream::UTF8FileStream (Path const & file_path, Error::Reporter & rep)
	: InputStream (rep)
	, m_file (nullptr)
{
	reporter().pushFileName (file_path);

	m_file = fopen (file_path.c_str(), "rb");
	if (nullptr == m_file)
	{
		setError();
		reporter().newInputError (location(), 1, L"Cannot open input file.");
	}
	else
		pop ();
}

//----------------------------------------------------------------------

UTF8FileStream::~UTF8FileStream ()
{
	if (nullptr != m_file)
		fclose (m_file);
	reporter().popFileName ();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

Char UTF8FileStream::readOne ()
{
	if (error() || eoi())
		return InvalidChar();

	assert (nullptr != m_file);

	auto sb = readStartByte ();
	if (eoi() || error() || sb < 0 || sb > 255)
		return InvalidChar();

	auto dsb = DecodeStartByte (uint8_t(sb));
	uint64_t acc = dsb.second;
	for (int i = 0; i < dsb.first; ++i)
		acc = (acc << 6) | readContinuationByte();

	// Note: acc should not be larger than 0x10FFFF, but even that won't fit in a wchar_t. :-(

	if (msc_BOM == acc)		// Ignore a Byte Order Mark
		return readOne();
	else
		return Char(acc);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

int UTF8FileStream::readByte ()
{
	auto c = fgetc (m_file);
	if (EOF == c)
	{
		if (feof(m_file)) setEOI ();
		if (ferror(m_file)) setError ();
		return -1;
	}
	return c;
}

//----------------------------------------------------------------------

int UTF8FileStream::readStartByte ()
{
	auto c = readByte();
	if (eoi() || error())
		return c;
	if (!ValidStartByte(uint8_t(c)))
		reporter().newInputWarning (location(), 2, L"Suspicious UTF-8 byte sequence: invalid start byte.");
	while (!ValidStartByte(uint8_t(c)) && !eoi() && !error())
		c = readByte();

	return c;
}

//----------------------------------------------------------------------

unsigned UTF8FileStream::readContinuationByte ()
{
	auto c = readByte();
	if ((c & 0xC0) != 0x80)
	{
		ungetc (c, m_file);	// !!!
		reporter().newInputWarning (location(), 3, L"Invalid UTF-8 continuation byte detected.");
		return 0;
	}
	return c & 0x3F;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

bool UTF8FileStream::ValidStartByte (uint8_t start_byte)
{
	return !(((start_byte & 0xC0) == 0x80) || (start_byte > 0xF4));
}

//----------------------------------------------------------------------
// First element is the number of additional bytes to read,
// the second element is the bit values from this byte that contribute to the final character code
std::pair<int, unsigned> UTF8FileStream::DecodeStartByte (uint8_t sb)
{
	     if (sb < 0x80) return {0, sb};
	else if (sb < 0xC0) return {0, 0};			// This should not happen
	else if (sb < 0xE0) return {1, sb & 0x1F};
	else if (sb < 0xF0) return {2, sb & 0x0F};
	else if (sb < 0xF8) return {3, sb & 0x07};
	else if (sb < 0xFC) return {4, sb & 0x03};	// This should not happen
	else if (sb < 0xFE) return {5, sb & 0x01};	// This should not happen
	else return {6, 0};							// This should not happen
}

//----------------------------------------------------------------------
//======================================================================

}	// namespace UPL

//======================================================================
