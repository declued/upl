#pragma once

//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>

//======================================================================

namespace UPL {

//======================================================================

class InputStream
{
public:
	explicit InputStream (Error::Reporter & reporter)
		: m_reporter (reporter)
		, m_cur_loc ()
		, m_cur_char (InvalidChar())
		, m_eoi (false)
		, m_error (false)
	{}

	virtual ~InputStream () {}

	Error::Reporter const & reporter () const {return m_reporter;}
	Error::Reporter & reporter () {return m_reporter;}
	Location const & location () const {return m_cur_loc;}
	bool eoi () const {return m_eoi;}
	bool error () const {return m_error;}

	Char curr () const {return m_cur_char;}

	// Returns false on EOI or error
	bool pop ()
	{
		m_cur_char = readOne ();
		if (!eoi() && !error())
			m_cur_loc.feed (m_cur_char);
		return !error() && !eoi();
	}

	// Reads input while "f(curr())" is true or EOI/error is reached.
	// Returns the read string
	template <typename F>
	String readWhile (F const & f)
	{
		String ret;

		while (f(curr()))
		{
			ret += curr();
			if (!pop())
				break;
		}

		return ret;
	}

protected:
	void setEOI () {m_eoi = true;}
	void clearEOI () {m_eoi = false;}
	void setError () {m_error = true;}
	void clearError () {m_error = false;}

	virtual Char readOne () = 0;

private:
	Error::Reporter & m_reporter;
	Location m_cur_loc;
	Char m_cur_char;
	bool m_eoi;
	bool m_error;
};

//----------------------------------------------------------------------
//======================================================================

class UTF8FileStream
	: public InputStream
{
	static Char const msc_BOM = 0xFEFF;

public:
	UTF8FileStream (Path const & file_path, Error::Reporter & rep);
	virtual ~UTF8FileStream ();

protected:
	// Reads and decodes one UTF-8 character
	Char readOne () override;

private:
	int readByte ();
	int readStartByte ();
	unsigned readContinuationByte ();

	static bool ValidStartByte (uint8_t start_byte);
	static std::pair<int, unsigned> DecodeStartByte (uint8_t start_byte);

private:
	FILE * m_file;
};

//======================================================================

}	// namespace UPL

//======================================================================
