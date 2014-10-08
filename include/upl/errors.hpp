#pragma once

//======================================================================

#include <upl/common.hpp>

//======================================================================

namespace UPL {
	namespace Error {

//======================================================================

enum class Category
{
	Unknown,
	Internal,
	Input,
	Lexer,
	Parser,
	CodeGen,
	VM,
	Runtime,
	Other,
};

//----------------------------------------------------------------------

enum class Severity
{
	Note,
	Warning,
	Error,
	Fatal,
};

//----------------------------------------------------------------------

typedef int Number;

//======================================================================

class Record
{
public:
	Record (Path path, Location loc, Severity sev, Category cat, Number num, String message)
		: m_file (std::move(path))
		, m_location (std::move(loc))
		, m_severity (sev)
		, m_category (cat)
		, m_number (num)
		, m_message (std::move(message))
	{}

	Path const & file () const {return m_file;}
	Location const & location () const {return m_location;}
	Severity const & severity () const {return m_severity;}
	Category const & category () const {return m_category;}
	Number const & number () const {return m_number;}
	String const & message () const {return m_message;}

private:
	Path m_file;
	Location m_location;
	Severity m_severity;
	Category m_category;
	Number m_number;
	String m_message;
};

//======================================================================

class Reporter
{
public:
	Reporter ();

	Path const & currentFile () const {return m_file_name_stack.back();}
	int count () const {return int(m_reports.size());}
	std::vector<Record> const & reports () const {return m_reports;}

	void pushFileName (Path new_file_name);
	void popFileName ();

	void newReport (Location loc, Severity sev, Category cat, Number num, String msg = String());

	// Convenience functions:
	void newInternalNote (Location loc, Number num, String msg = String());
	void newInternalWarning (Location loc, Number num, String msg = String());
	void newInternalError (Location loc, Number num, String msg = String());
	void newInputNote (Location loc, Number num, String msg = String());
	void newInputWarning (Location loc, Number num, String msg = String());
	void newInputError (Location loc, Number num, String msg = String());
	void newLexerNote (Location loc, Number num, String msg = String());
	void newLexerWarning (Location loc, Number num, String msg = String());
	void newLexerError (Location loc, Number num, String msg = String());
	void newParserNote (Location loc, Number num, String msg = String());
	void newParserWarning (Location loc, Number num, String msg = String());
	void newParserError (Location loc, Number num, String msg = String());
	void newCodeGenNote (Location loc, Number num, String msg = String());
	void newCodeGenWarning (Location loc, Number num, String msg = String());
	void newCodeGenError (Location loc, Number num, String msg = String());
	void newVMNote (Location loc, Number num, String msg = String());
	void newVMWarning (Location loc, Number num, String msg = String());
	void newVMError (Location loc, Number num, String msg = String());
	void newRuntimeNote (Location loc, Number num, String msg = String());
	void newRuntimeWarning (Location loc, Number num, String msg = String());
	void newRuntimeError (Location loc, Number num, String msg = String());

private:
	std::vector<Record> m_reports;
	std::vector<Path> m_file_name_stack;
};

//----------------------------------------------------------------------
//======================================================================

	}	// namespace Error
}	// namespace UPL

//======================================================================

