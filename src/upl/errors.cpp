//======================================================================

#include <upl/errors.hpp>

//======================================================================

namespace UPL {
	namespace Error {

//======================================================================
//----------------------------------------------------------------------
//======================================================================

Reporter::Reporter ()
	: m_reports ()
	, m_file_name_stack ()
{
	m_file_name_stack.emplace_back (L"???");
}

//----------------------------------------------------------------------

void Reporter::pushFileName (Path new_file_name)
{
	m_file_name_stack.emplace_back (std::move(new_file_name));
}

//----------------------------------------------------------------------

void Reporter::popFileName ()
{
	if (m_file_name_stack.size() > 1)
		m_file_name_stack.pop_back ();
}

//----------------------------------------------------------------------

void Reporter::newReport (Location loc, Severity sev, Category cat, Number num, String msg /*= String()*/)
{
	m_reports.emplace_back (currentFile(), std::move(loc), sev, cat, num, std::move(msg));
}

//----------------------------------------------------------------------

#define IMPLEMENT_CONVENIENCE_REPORT_METHOD(cat, sev)									\
	void Reporter::new##cat##sev (Location loc, Number num, String msg) {				\
		newReport (std::move(loc), Severity::sev, Category::cat, num, std::move(msg));	\
	}

IMPLEMENT_CONVENIENCE_REPORT_METHOD(Internal, Note)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Internal, Warning)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Internal, Error)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Input, Note)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Input, Warning)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Input, Error)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Lexer, Note)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Lexer, Warning)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Lexer, Error)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Parser, Note)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Parser, Warning)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Parser, Error)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(CodeGen, Note)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(CodeGen, Warning)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(CodeGen, Error)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(VM, Note)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(VM, Warning)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(VM, Error)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Runtime, Note)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Runtime, Warning)
IMPLEMENT_CONVENIENCE_REPORT_METHOD(Runtime, Error)

#undef IMPLEMENT_CONVENIENCE_REPORT_METHOD

//======================================================================

	}	// namespace Error
}	// namespace UPL

//======================================================================
