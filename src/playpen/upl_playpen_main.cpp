//======================================================================

#include <upl/st_code.hpp>

#include <upl/lexer.hpp>
#include <upl/input.hpp>
#include <upl/errors.hpp>
#include <upl/common.hpp>

#include <cstdint>
#include <iostream>

//======================================================================
//======================================================================
//----------------------------------------------------------------------
//======================================================================

void ReportErrors (UPL::Error::Reporter const & err);
void TestStringConversions ();
void TestInputStream ();
void TestLexer ();
void TestSTCode ();

//======================================================================

int main (int /*argc*/, char * /*argv*/[])
{
	std::cout << "This is the UPL playpen, where testing and tinkering happens.\n";
	std::cout << std::endl;

	std::cout << "==========================" << std::endl;
	std::cout << "Testing string conversions" << std::endl;
	std::cout << "--------------------------" << std::endl;
	TestStringConversions ();
	std::cout << std::endl;

	std::cout << "=====================" << std::endl;
	std::cout << "Testing input streams" << std::endl;
	std::cout << "---------------------" << std::endl;
	TestInputStream ();
	std::cout << std::endl;

	std::cout << "=================" << std::endl;
	std::cout << "Testing the lexer" << std::endl;
	std::cout << "-----------------" << std::endl;
	TestLexer ();
	std::cout << std::endl;

	std::cout << "====================" << std::endl;
	std::cout << "Testing the ST Codec" << std::endl;
	std::cout << "--------------------" << std::endl;
	TestSTCode ();
	std::cout << std::endl;

	return 0;
}

//======================================================================

void ReportErrors (UPL::Error::Reporter const & err)
{
	using std::wcout;
	using std::cout;
	using std::endl;

	wcout << "Errors: " << err.count() << endl;
	for (auto const & er : err.reports())
		wcout
			<< "  "
			<< UPL::ToString(er.file()) << ":" << er.location().line() << "," << er.location().column()
			<< " (" << int(er.category()) << "," << int(er.severity())
			<< ") : (" << er.number() << ") "
			<< er.message()
			<< endl;
}

//----------------------------------------------------------------------

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

	if (!inp.eoi() && !inp.error())
		do {
			wcout << ((inp.curr() < 32 || inp.curr() > 255) ? L'?' : inp.curr());
			wcout << " (0x" << hex << int(inp.curr()) << dec << ")";
			wcout << " @" << inp.location().line() << "," << inp.location().column();
			wcout << endl;
		} while (inp.pop());

	wcout << endl;
	ReportErrors (err);
	wcout << endl;
}

//----------------------------------------------------------------------

void TestLexer ()
{
	using std::wcout;
	using std::endl;

	UPL::Error::Reporter err;
	UPL::UTF8FileStream inp ("sample-program-00.upl", err);
	UPL::Lexer lex (inp, err);

	if (!lex.eoi() && !lex.error())
		do {
			UPL::Token const & t = lex.curr();
			wcout
				<< "@" << t.location().line() << "," << t.location().column()
				<< " : " << t.typeStr() << " (" << t.uncookedValue() << ")"
				<< endl;
		} while (lex.pop());

	wcout << endl;
	ReportErrors (err);
	wcout << endl;
}

//----------------------------------------------------------------------

void TestSTCode ()
{
	using std::wcout;
	using std::endl;
	using ST = UPL::Type::STCode;
	using UPL::Type::Tag;

	uint32_t test_ints [] = {2, 0, 7, 8, 15, 16, 255, 256, 4095, 4096, 24543563, 3000000000U};

	for (auto ti : test_ints)
	{
		auto ci = UPL::Type::STCode::SerializeInt(ti);
		wcout << ti << " -> (" << ci.size() << ") ";
		//for (auto b : ci) wcout << " " << int(b);
		for (auto b : ci) wcout << (L"0123456789ABCDEF")[int(b)];
		wcout << endl;
	}

	wcout << endl;

	UPL::Type::STContainer reg;

	auto ir0 = ST::MakeVariant(false, {reg.byTag(Tag::Int), reg.byTag(Tag::Nil), reg.byTag(Tag::String), reg.byTag(Tag::Any)});
	auto p0 = ST::Pack(ir0);
	auto id0 = reg.createType(p0);

	auto ir1 = ST::MakeFuction(false, id0, {id0, id0, id0, reg.byTag(Tag::Bool)});
	auto p1 = ST::Pack(ir1);
	auto id1 = reg.createType(p1);

	auto ir2 = ST::MakeMap(false, id1, id0);
	auto p2 = ST::Pack(ir2);
	auto id2 = reg.createType(p2);

	auto ir3 = ST::MakeArray(true, 2000000000, id1);
	auto p3 = ST::Pack(ir3);
	auto id3 = reg.createType(p3);

	auto ir4 = ST::MakeMap(false, id1, id0);
	auto p4 = ST::Pack(ir4);
	auto id4 = reg.createType(p4);
	assert (id4 == id2);

	auto ir5 = ST::MakeArray(true, 2000000000, id1);
	auto p5 = ST::Pack(ir5);
	auto id5 = reg.createType(p5);
	assert (id5 == id3);

	UPL::Type::STIR ir [] = {ir0, ir1, ir2, ir3, ir4, ir5};
	UPL::Type::PackedST p [] = {p0, p1, p2, p3, p4, p5};
	UPL::Type::ID id [] = {id0, id1, id2, id3, id4, id5};

	wcout << endl;
	for (int i = 0; i < 6; ++i)
	{
		wcout << "(" << ir[i].size() << ")";
		for (auto b : ir[i]) wcout << " " << std::hex << b;
		wcout << " , ";
		wcout << "(" << p[i].size() << ")";
		for (auto b : p[i]) wcout << " " << std::hex << b;
		wcout << " , ";
		wcout << std::dec << id[i] << endl;
	}

	wcout << endl;
	for (unsigned i = 0; i < reg.size(); ++i)
	{
		wcout << i << " : ";
		auto u = reg.unpack(i);
		wcout
			<< int(u.tag) << ", " << u.is_const << ", "
			<< u.type1 << ", " << u.type2 << ", " << u.size << ", "
			<< "(";
		for (auto v : u.type_list)
			wcout << " " << v;
		wcout << ")" << endl;
	}
}

//----------------------------------------------------------------------
//======================================================================
