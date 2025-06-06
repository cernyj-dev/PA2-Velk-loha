// #ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>
#include "expression.h"

#include "headers/CPos.hpp"
#include "headers/CNode.hpp"
#include "headers/CMyOwnExprBuilder.hpp"
#include "headers/CSpreadsheet.hpp"

using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

// constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
// constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
// constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
// constexpr unsigned SPREADSHEET_SPEED = 0x08;
// constexpr unsigned SPREADSHEET_PARSER = 0x10;
// #endif /* __PROGTEST__ */

using namespace std;

// Comments relatable to the start of the project - I dropped using them consistently later on

// double line - class seperator
// single line - method/function separator
// [xxx_PROBLEM] - potentional problem down the line, xxx will typically be low/mid/high perceived significance
// exception messages - <specific problem>: <where the was it thrown>
// [PRINT] - my own //cout for checking whether or not the program does what its supposed to

ostream &operator<<(ostream &os, const CValue &rhs)
{
	if (holds_alternative<string>(rhs))
	{
		if (get<string>(rhs).empty())
		{
			os << "string empty";
		}
		else
		{
			os << get<string>(rhs);
		}
	}
	else if (holds_alternative<double>(rhs))
	{
		os << get<double>(rhs);
	}
	else
	{
		os << ", void";
	}
	return os;
}

bool valueMatch(const CValue &r,
				const CValue &s)

{
	if (r.index() != s.index())
		return false;
	if (r.index() == 0)
		return true;
	if (r.index() == 2)
		return std::get<std::string>(r) == std::get<std::string>(s);
	if (std::isnan(std::get<double>(r)) && std::isnan(std::get<double>(s)))
		return true;
	if (std::isinf(std::get<double>(r)) && std::isinf(std::get<double>(s)))
		return (std::get<double>(r) < 0 && std::get<double>(s) < 0) || (std::get<double>(r) > 0 && std::get<double>(s) > 0);
	return fabs(std::get<double>(r) - std::get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(std::get<double>(r));
}

void testing(bool test1, bool test2 = false, bool test3 = false)
{
	CSpreadsheet x0, x1;
	std::ostringstream oss;
	std::istringstream iss;
	std::string data;

	// test1 - my own simple tests to see how the program behaves
	if (test1)
	{
		// std:://cout << "test" << std::endl; //[PRINT]
		// try
		// {
		// 	CPos x("bob60");
		// }
		// catch (const exception &e)
		// {
		// 	//cout << "Exception has occured: " << e.what() << endl; //[PRINT]
		// }

		CSpreadsheet bob;
		// bob.setCell(CPos("A1"), "=2");
		// // bob.setCell(CPos("B77"), "=A1 + B2");
		// bob.setCell(CPos("A76"), "=A$1 / 2");
		// //cout << "A76: " << bob.getValue(CPos("A76")) << endl; //[PRINT]
		// 													  // bob.setCell(CPos("A76"), "");

		// bob.setCell(CPos("A2"), "=\"test\" = \"test\"");
		// //cout << "A2: " << bob.getValue(CPos("A2")) << endl; //[PRINT]
		bob.setCell(CPos("A0"), "= \"ahoj\"\"ahoj\" + \"ahoj\"\"ahoj\"");
		cout << "A0: " << bob.getValue(CPos("A0")) << endl;

		bob.setCell(CPos("A1"), "\"=\"");
		cout << "A1: " << bob.getValue(CPos("A1")) << endl;

		bob.setCell(CPos("A2"), "=\"\"\"=\"\"\"");
		cout << "A2: " << bob.getValue(CPos("A2")) << endl;

		bob.print();

		// bob.setCell(CPos("A7"), "60");
		// bob.setCell(CPos("A8"), "=A6 + A7");
		// // cout << "A8: " << bob.getValue(CPos("A8")) << endl; //[PRINT]

		// bob.setCell(CPos("A1"), "30");
		// bob.setCell(CPos("B2"), "20");
		// bob.setCell(CPos("C3"), "=$A$1");
		// // cout << "A1: " << bob.getValue(CPos("A1")) << " | B2: " << bob.getValue(CPos("B2")) << " | C3: " << bob.getValue(CPos("C3")) << endl; //[PRINT]
		// bob.copyRect(CPos("D4"), CPos("C3"), 1, 1);
		// // cout << "D4: " << bob.getValue(CPos("D4")) << endl; //[PRINT]
		// bob.print();

		// CSpreadsheet kok;
		// oss.str("");
		// assert(bob.save(oss));
		// std::cout << "oss: \n"
		// 		  << oss.str() << std::endl; //[PRINT]

		// data = oss.str();
		// iss.clear();
		// iss.str(data);
		// assert(kok.load(iss));
		// kok.print();
	}
	if (test2)
	{
		assert(x0.setCell(CPos("A1"), "10"));
		assert(x0.setCell(CPos("A2"), "20.5"));
		assert(x0.setCell(CPos("A3"), "3e1"));
		assert(x0.setCell(CPos("A4"), "=40"));
		assert(x0.setCell(CPos("A5"), "=5e+1"));
		assert(x0.setCell(CPos("A6"), "raw text with any characters, including a quote \" or a newline\n"));
		assert(x0.setCell(CPos("A7"), "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\""));
		cout << "A7: " << x0.getValue(CPos("A7")) << endl;
		assert(valueMatch(x0.getValue(CPos("A1")), CValue(10.0)));
		assert(valueMatch(x0.getValue(CPos("A2")), CValue(20.5)));
		assert(valueMatch(x0.getValue(CPos("A3")), CValue(30.0)));
		assert(valueMatch(x0.getValue(CPos("A4")), CValue(40.0)));
		assert(valueMatch(x0.getValue(CPos("A5")), CValue(50.0)));
		assert(valueMatch(x0.getValue(CPos("A6")), CValue("raw text with any characters, including a quote \" or a newline\n")));
		assert(valueMatch(x0.getValue(CPos("A7")), CValue("quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++.")));
		assert(valueMatch(x0.getValue(CPos("A8")), CValue()));
		assert(valueMatch(x0.getValue(CPos("AAAA9999")), CValue()));
		assert(x0.setCell(CPos("B1"), "=A1+A2*A3"));
		assert(x0.setCell(CPos("B2"), "= -A1 ^ 2 - A2 / 2   "));
		assert(x0.setCell(CPos("B3"), "= 2 ^ $A$1"));
		assert(x0.setCell(CPos("B4"), "=($A1+A$2)^2"));
		assert(x0.setCell(CPos("B5"), "=B1+B2+B3+B4"));
		assert(x0.setCell(CPos("B6"), "=B1+B2+B3+B4+B5"));
		assert(valueMatch(x0.getValue(CPos("B1")), CValue(625.0)));
		assert(valueMatch(x0.getValue(CPos("B2")), CValue(-110.25)));
		assert(valueMatch(x0.getValue(CPos("B3")), CValue(1024.0)));
		assert(valueMatch(x0.getValue(CPos("B4")), CValue(930.25)));
		assert(valueMatch(x0.getValue(CPos("B5")), CValue(2469.0)));
		assert(valueMatch(x0.getValue(CPos("B6")), CValue(4938.0)));
		assert(x0.setCell(CPos("A1"), "12"));
		assert(valueMatch(x0.getValue(CPos("B1")), CValue(627.0)));
		assert(valueMatch(x0.getValue(CPos("B2")), CValue(-154.25)));
		assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
		assert(valueMatch(x0.getValue(CPos("B4")), CValue(1056.25)));
		assert(valueMatch(x0.getValue(CPos("B5")), CValue(5625.0)));
		assert(valueMatch(x0.getValue(CPos("B6")), CValue(11250.0)));
		// //cout << "-------------------------------------------------------------------" << endl;
		x1 = x0;
		// //cout << "-------------------------------------------------------------------" << endl;
		assert(x0.setCell(CPos("A2"), "100"));
		assert(x1.setCell(CPos("A2"), "=A3+A5+A4"));
		assert(valueMatch(x0.getValue(CPos("B1")), CValue(3012.0)));
		assert(valueMatch(x0.getValue(CPos("B2")), CValue(-194.0)));
		assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
		assert(valueMatch(x0.getValue(CPos("B4")), CValue(12544.0)));
		assert(valueMatch(x0.getValue(CPos("B5")), CValue(19458.0)));
		assert(valueMatch(x0.getValue(CPos("B6")), CValue(38916.0)));
		assert(valueMatch(x1.getValue(CPos("B1")), CValue(3612.0)));
		assert(valueMatch(x1.getValue(CPos("B2")), CValue(-204.0)));
		assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
		assert(valueMatch(x1.getValue(CPos("B4")), CValue(17424.0)));
		assert(valueMatch(x1.getValue(CPos("B5")), CValue(24928.0)));
		assert(valueMatch(x1.getValue(CPos("B6")), CValue(49856.0)));
	}
	if (test3)
	{
		oss.clear();
		oss.str("");
		assert(x0.save(oss));
		data = oss.str();
		iss.clear();
		iss.str(data);
		// cout << "-------------------------------------------------------------------" << endl;
		// std:: cout << "oss: \n"
		//	<< oss.str() << std::endl; //[PRINT]
		// cout << "-------------------------------------------------------------------" << endl;
		assert(x1.load(iss));
		// cout << "-------------------------------------------------------------------" << endl;
		//  x1 = x0;
		// cout << "-------------------------------------------------------------------" << endl;
		// cout << "B1: " << x1.getValue(CPos("B1")) << endl; //[PRINT]
		assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
		// cout << "B2: " << x1.getValue(CPos("B2")) << endl; //[PRINT]
		assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
		assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
		assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
		assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
		assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
		assert(x0.setCell(CPos("A3"), "4e1"));
		assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
		assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
		assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
		assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
		assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
		assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
		oss.clear();
		oss.str("");
		assert(x0.save(oss));
		data = oss.str();
		for (size_t i = 0; i < std::min<size_t>(data.length(), 10); i++)
			data[i] ^= 0x5a;
		iss.clear();
		iss.str(data);
		assert(!x1.load(iss));
		x0.print();
		assert(x0.setCell(CPos("D0"), "10"));
		assert(x0.setCell(CPos("D1"), "20"));
		assert(x0.setCell(CPos("D2"), "30"));
		assert(x0.setCell(CPos("D3"), "40"));
		assert(x0.setCell(CPos("D4"), "50"));
		assert(x0.setCell(CPos("E0"), "60"));
		assert(x0.setCell(CPos("E1"), "70"));
		assert(x0.setCell(CPos("E2"), "80"));
		assert(x0.setCell(CPos("E3"), "90"));
		assert(x0.setCell(CPos("E4"), "100"));
		assert(x0.setCell(CPos("F10"), "=D0+5"));
		assert(x0.setCell(CPos("F11"), "=$D0+5"));
		assert(x0.setCell(CPos("F12"), "=D$0+5"));
		assert(x0.setCell(CPos("F13"), "=$D$0+5"));
		x0.copyRect(CPos("G11"), CPos("F10"), 1, 4);
		x0.print();
		assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
		assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
		assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
		assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
		assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
		assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
		x0.print();
		x0.copyRect(CPos("G11"), CPos("F10"), 2, 4);
		x0.print();
		assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
		assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
		assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
		assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
		assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
		assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
		assert(valueMatch(x0.getValue(CPos("H10")), CValue()));
		assert(valueMatch(x0.getValue(CPos("H11")), CValue()));
		assert(valueMatch(x0.getValue(CPos("H12")), CValue()));
		assert(valueMatch(x0.getValue(CPos("H13")), CValue(35.0)));
		assert(valueMatch(x0.getValue(CPos("H14")), CValue()));
		x0.print();
		assert(x0.setCell(CPos("F0"), "-27"));
		x0.print();
		assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
		x0.copyRect(CPos("H12"), CPos("H13"), 1, 2);
		assert(valueMatch(x0.getValue(CPos("H12")), CValue(25.0)));
		assert(valueMatch(x0.getValue(CPos("H13")), CValue(-22.0)));
		assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
		x0.print();
	}
}

int main()
{
	testing(false, true, true);
	return 0;
}

// #ifndef __PROGTEST__

// bool valueMatch(const CValue &r,
// 				const CValue &s)

// {
// 	if (r.index() != s.index())
// 		return false;
// 	if (r.index() == 0)
// 		return true;
// 	if (r.index() == 2)
// 		return std::get<std::string>(r) == std::get<std::string>(s);
// 	if (std::isnan(std::get<double>(r)) && std::isnan(std::get<double>(s)))
// 		return true;
// 	if (std::isinf(std::get<double>(r)) && std::isinf(std::get<double>(s)))
// 		return (std::get<double>(r) < 0 && std::get<double>(s) < 0) || (std::get<double>(r) > 0 && std::get<double>(s) > 0);
// 	return fabs(std::get<double>(r) - std::get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(std::get<double>(r));
// }

// int main()
// {
// 	CSpreadsheet x0, x1;
// 	std::ostringstream oss;
// 	std::istringstream iss;
// 	std::string data;
// 	assert(x0.setCell(CPos("A1"), "10"));
// 	assert(x0.setCell(CPos("A2"), "20.5"));
// 	assert(x0.setCell(CPos("A3"), "3e1"));
// 	assert(x0.setCell(CPos("A4"), "=40"));
// 	assert(x0.setCell(CPos("A5"), "=5e+1"));
// 	assert(x0.setCell(CPos("A6"), "raw text with any characters, including a quote \" or a newline\n"));
// 	assert(x0.setCell(CPos("A7"), "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\""));
// 	assert(valueMatch(x0.getValue(CPos("A1")), CValue(10.0)));
// 	assert(valueMatch(x0.getValue(CPos("A2")), CValue(20.5)));
// 	assert(valueMatch(x0.getValue(CPos("A3")), CValue(30.0)));
// 	assert(valueMatch(x0.getValue(CPos("A4")), CValue(40.0)));
// 	assert(valueMatch(x0.getValue(CPos("A5")), CValue(50.0)));
// 	assert(valueMatch(x0.getValue(CPos("A6")), CValue("raw text with any characters, including a quote \" or a newline\n")));
// 	assert(valueMatch(x0.getValue(CPos("A7")), CValue("quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++.")));
// 	assert(valueMatch(x0.getValue(CPos("A8")), CValue()));
// 	assert(valueMatch(x0.getValue(CPos("AAAA9999")), CValue()));
// 	assert(x0.setCell(CPos("B1"), "=A1+A2*A3"));
// 	assert(x0.setCell(CPos("B2"), "= -A1 ^ 2 - A2 / 2   "));
// 	assert(x0.setCell(CPos("B3"), "= 2 ^ $A$1"));
// 	assert(x0.setCell(CPos("B4"), "=($A1+A$2)^2"));
// 	assert(x0.setCell(CPos("B5"), "=B1+B2+B3+B4"));
// 	assert(x0.setCell(CPos("B6"), "=B1+B2+B3+B4+B5"));
// 	assert(valueMatch(x0.getValue(CPos("B1")), CValue(625.0)));
// 	assert(valueMatch(x0.getValue(CPos("B2")), CValue(-110.25)));
// 	assert(valueMatch(x0.getValue(CPos("B3")), CValue(1024.0)));
// 	assert(valueMatch(x0.getValue(CPos("B4")), CValue(930.25)));
// 	assert(valueMatch(x0.getValue(CPos("B5")), CValue(2469.0)));
// 	assert(valueMatch(x0.getValue(CPos("B6")), CValue(4938.0)));
// 	assert(x0.setCell(CPos("A1"), "12"));
// 	assert(valueMatch(x0.getValue(CPos("B1")), CValue(627.0)));
// 	assert(valueMatch(x0.getValue(CPos("B2")), CValue(-154.25)));
// 	assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
// 	assert(valueMatch(x0.getValue(CPos("B4")), CValue(1056.25)));
// 	assert(valueMatch(x0.getValue(CPos("B5")), CValue(5625.0)));
// 	assert(valueMatch(x0.getValue(CPos("B6")), CValue(11250.0)));
// 	x1 = x0;
// 	//cout << "-------------------------------------------------------------------" << endl;
// 	//cout << "x0 B1: " << x0.getValue(CPos("B1")) << endl;
// 	//cout << "x1 B1: " << x1.getValue(CPos("B1")) << endl;

// 	//cout << "-------------------------------------------------------------------" << endl;
// 	assert(x0.setCell(CPos("A2"), "100"));
// 	assert(x1.setCell(CPos("A2"), "=A3+A5+A4"));
// 	//cout << "-------------------------------------------------------------------" << endl;
// 	//cout << "x0 B1: " << x0.getValue(CPos("A2")) << endl;
// 	//cout << "x1 B1: " << x1.getValue(CPos("A2")) << endl;
// 	//cout << "-------------------------------------------------------------------" << endl;

// 	//cout << "x0 A1: " << x0.getValue(CPos("A1")) << endl;
// 	//cout << "x0 A2: " << x0.getValue(CPos("A2")) << endl;
// 	//cout << "x0 A3: " << x0.getValue(CPos("A3")) << endl;

// 	//cout << "x1 A1: " << x1.getValue(CPos("A1")) << endl;
// 	//cout << "x1 A2: " << x1.getValue(CPos("A2")) << endl;
// 	//cout << "x1 A3: " << x1.getValue(CPos("A3")) << endl;

// 	//cout << "-------------------------------------------------------------------" << endl;

// 	//cout << "x0 B1: " << x0.getValue(CPos("B1")) << endl;
// 	//cout << "x1 B1: " << x1.getValue(CPos("B1")) << endl;
// 	//cout << "-------------------------------------------------------------------" << endl;

// 	assert(valueMatch(x0.getValue(CPos("B1")), CValue(3012.0)));
// 	assert(valueMatch(x0.getValue(CPos("B2")), CValue(-194.0)));
// 	assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
// 	assert(valueMatch(x0.getValue(CPos("B4")), CValue(12544.0)));
// 	assert(valueMatch(x0.getValue(CPos("B5")), CValue(19458.0)));
// 	assert(valueMatch(x0.getValue(CPos("B6")), CValue(38916.0)));
// 	assert(valueMatch(x1.getValue(CPos("B1")), CValue(3612.0))); 
// 	assert(valueMatch(x1.getValue(CPos("B2")), CValue(-204.0)));
// 	assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
// 	assert(valueMatch(x1.getValue(CPos("B4")), CValue(17424.0)));
// 	assert(valueMatch(x1.getValue(CPos("B5")), CValue(24928.0)));
// 	assert(valueMatch(x1.getValue(CPos("B6")), CValue(49856.0)));
// 	oss.clear();
// 	// oss.str("");
// 	// assert(x0.save(oss));
// 	// data = oss.str();
// 	// iss.clear();
// 	// iss.str(data);
// 	// assert(x1.load(iss));
// 	// assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
// 	// assert(x0.setCell(CPos("A3"), "4e1"));
// 	// assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
// 	// assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
// 	// oss.clear();
// 	// oss.str("");
// 	// assert(x0.save(oss));
// 	// data = oss.str();
// 	// for (size_t i = 0; i < std::min<size_t>(data.length(), 10); i++)
// 	// 	data[i] ^= 0x5a;
// 	// iss.clear();
// 	// iss.str(data);
// 	// assert(!x1.load(iss));
// 	// assert(x0.setCell(CPos("D0"), "10"));
// 	// assert(x0.setCell(CPos("D1"), "20"));
// 	// assert(x0.setCell(CPos("D2"), "30"));
// 	// assert(x0.setCell(CPos("D3"), "40"));
// 	// assert(x0.setCell(CPos("D4"), "50"));
// 	// assert(x0.setCell(CPos("E0"), "60"));
// 	// assert(x0.setCell(CPos("E1"), "70"));
// 	// assert(x0.setCell(CPos("E2"), "80"));
// 	// assert(x0.setCell(CPos("E3"), "90"));
// 	// assert(x0.setCell(CPos("E4"), "100"));
// 	// assert(x0.setCell(CPos("F10"), "=D0+5"));
// 	// assert(x0.setCell(CPos("F11"), "=$D0+5"));
// 	// assert(x0.setCell(CPos("F12"), "=D$0+5"));
// 	// assert(x0.setCell(CPos("F13"), "=$D$0+5"));
// 	// x0.copyRect(CPos("G11"), CPos("F10"), 1, 4);
// 	// assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
// 	// assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
// 	// assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
// 	// assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
// 	// assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
// 	// assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
// 	// x0.copyRect(CPos("G11"), CPos("F10"), 2, 4);
// 	// assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
// 	// assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
// 	// assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
// 	// assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
// 	// assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
// 	// assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
// 	// assert(valueMatch(x0.getValue(CPos("H10")), CValue()));
// 	// assert(valueMatch(x0.getValue(CPos("H11")), CValue()));
// 	// assert(valueMatch(x0.getValue(CPos("H12")), CValue()));
// 	// assert(valueMatch(x0.getValue(CPos("H13")), CValue(35.0)));
// 	// assert(valueMatch(x0.getValue(CPos("H14")), CValue()));
// 	// assert(x0.setCell(CPos("F0"), "-27"));
// 	// assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
// 	// x0.copyRect(CPos("H12"), CPos("H13"), 1, 2);
// 	// assert(valueMatch(x0.getValue(CPos("H12")), CValue(25.0)));
// 	// assert(valueMatch(x0.getValue(CPos("H13")), CValue(-22.0)));
// 	// assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
// 	return EXIT_SUCCESS;
// }
// // #endif /* __PROGTEST__ */
