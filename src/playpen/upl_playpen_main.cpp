//======================================================================

#include <upl/common.hpp>

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
	TestStringConversions ();
	std::cout << std::endl;


	return 0;
}

//======================================================================

void TestStringConversions ()
{
	using std::cout;
	using std::endl;
	using std::wcout;

	wcout << UPL::ToString(-14414) << endl;
	wcout << UPL::ToString(-14414LL) << endl;
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
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//======================================================================
