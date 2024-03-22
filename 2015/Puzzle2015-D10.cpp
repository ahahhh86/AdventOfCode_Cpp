module;

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D10;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	std::string getInput(const std::vector<std::string> in)
	{
		if (in.size() != 1) throw AOC::InvalidInputData("getInput");
		if (in[0].find_first_not_of("0123456789") != std::string::npos) throw AOC::InvalidInputData("getInput");

		return in[0];
	}
}



namespace { // Calculations
	std::string getNextString(std::string_view str)
	{
		std::string result{};
		result.reserve(str.length() * 2);

		int count{1};
		char buffer{str[0]};

		for (auto i{std::next(str.cbegin())}; i != str.cend(); ++i) {
			if (*i == buffer) {
				++count;
			} else {
				result.append(std::to_string(count));
				result.push_back(buffer);

				count = 1;
				buffer = *i;
			}
		}
		result.append(std::to_string(count));
		result.push_back(buffer);

		return result;
	}



	auto calculateStrings(std::string& str, int loops)
	{
		for (int i{0}; i < loops; ++i) {
			str = getNextString(str);
		}

		return str.length();
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			using namespace std::literals;
			io.printTest(getNextString("211"s), "1221"s);

			std::string str{"1"s};
			str = getNextString(str);
			io.printTest(str, "11"s);
			str = getNextString(str);
			io.printTest(str, "21"s);
			str = getNextString(str);
			io.printTest(str, "1211"s);
			str = getNextString(str);
			io.printTest(str, "111221"s);
			str = getNextString(str);
			io.printTest(str, "312211"s);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D10 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d10}};
		testPuzzle(io);

		constexpr int part1loops{40};
		constexpr int part2extraloops{10};

		auto input{getInput(io.readInputFile<std::string>())};
		io.printSolution(calculateStrings(input, part1loops), 252594ULL);
		io.printSolution(calculateStrings(input, part2extraloops), 3579328ULL);
	}
}