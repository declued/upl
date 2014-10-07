//======================================================================

#include <upl/common.hpp>
#include <upl/errors.hpp>
#include <upl/input.hpp>

#include <cstdint>
#include <iostream>

//======================================================================
//======================================================================
//----------------------------------------------------------------------
//======================================================================

void TestStringConversions ();
void TestInputStream ();

//======================================================================

int main (int /*argc*/, char * /*argv*/[])
{
	std::cout << "This is the UPL playpen, where testing and tinkering happens.\n";
	std::cout << std::endl;

	std::cout << "Testing string conversions: " << std::endl;
	TestStringConversions ();
	std::cout << std::endl;

	std::cout << "Testing input streams: " << std::endl;
	TestInputStream ();
	std::cout << std::endl;


	return 0;
}

//======================================================================

void TestStringConversions ()
{
	using std::wcout;
	using std::cout;
	using std::endl;

	wcout << UPL::ToString(-14414) << endl;
	wcout << UPL::ToString(int64_t(-14414LL)) << endl;
	wcout << UPL::ToString(-14414.0f) << endl;
	wcout << UPL::ToString(-14414.0) << endl;
	wcout << UPL::ToString(false) << endl;
	wcout << UPL::ToString("Hello, world!") << endl;
	wcout << endl;
	wcout << UPL::FromString<int>(L"-123456") << endl;
	wcout << UPL::FromString<uint64_t>(L"123456789012345") << endl;
	 cout << UPL::FromString<std::string>(L"Hello, world!") << endl;
}

//----------------------------------------------------------------------

void TestInputStream ()
{
	using std::wcout;
	using std::endl;
	using std::hex;
	using std::dec;

	UPL::Error::Reporter err;
	UPL::UTF8FileStream inp ("sample-utf8-input.txt", err);

	do {
		wcout << ((inp.curr() < 32 || inp.curr() > 255) ? L'?' : inp.curr());
		wcout << " (0x" << hex << int(inp.curr()) << dec << ")";
		wcout << " @" << inp.location().line() << "," << inp.location().column();
		wcout << endl;
	} while (inp.pop());

	wcout << endl;
	wcout << "Errors: " << err.count() << endl;
	for (auto const & er : err.reports())
		wcout
			<< "  "
			<< UPL::ToString(er.file()) << ":" << er.location().line() << "," << er.location().column()
			<< " (" << int(er.category()) << "," << int(er.severity())
			<< ") : (" << er.number() << ") "
			<< er.message()
			<< endl;
	wcout << endl;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//======================================================================
