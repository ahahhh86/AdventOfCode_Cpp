module;

#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <Windows.h>

module IO;

//import std; // TODO: import if IntelliSense works with it
import AdventDate;
import Errors;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC {
	// thanks to https://cplusplus.com/forum/general/48837/
	void toClipboard(const std::string& s)
	{
		const HWND hwnd{GetDesktopWindow()};
		OpenClipboard(hwnd);
		EmptyClipboard();
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
		if (!hg) {
			CloseClipboard();
			return;
		}
		memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);
	}



	constexpr std::string_view horizontalLine{"\t--------------------------------------------------------------------------------------------"};

	IO::IO(Date date):
		m_date{date}
	{
		printDate(date);
		std::println("");
	}



	IO::~IO()
	{
		std::print("\n- - - - - - - - - - - - - - - - - - - -\n\n\n");
	}



	void IO::startTests() const
	{
		constexpr std::string_view testOutputFormat{"\t{:^16} {:^6} | {:>16} {:>16} | {:^11}[s]"};
		std::println(testOutputFormat, "result", "ID", "result", "expected", "Time");
		std::println(horizontalLine);
	}



	void IO::printMsg(std::string_view str) const
	{
		std::cout << str << '\n';
	}



	void IO::endTests()
	{
		std::println(horizontalLine);
		if (m_failedTests == 0) {
			std::println("\tsuccess :)");
		} else {
			std::println("\t*** {} tests failed ***", m_failedTests);
		}
		std::println("");
		m_stopwatch.reset();
	}



	std::ifstream IO::getInputFile() const
	{
		const std::string filename{std::format("./input/{}/Day{:02}.txt", +m_date.year, +m_date.day)};
		std::ifstream result{filename};
		if (!result) { throw InvalidFile{filename}; }
		return result;
	}
}