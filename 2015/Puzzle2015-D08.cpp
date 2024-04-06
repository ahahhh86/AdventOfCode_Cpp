module;

#include <algorithm>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D08;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using SantasList = std::vector<std::string>;

	void validate(const SantasList& list)
	{
		std::size_t line{1};
		std::ranges::for_each(list, [&](std::string_view str) {
			if (!str.starts_with('"') || !str.ends_with('"')) { throw AOC::InvalidFileLine{line}; }
			++line;
		});
	}
}



namespace Part1 { // Calculations
	namespace {
		auto countStr(std::string_view findStr, std::string_view str)
		{
			int result{0};
			std::size_t pos{str.find(findStr)};

			while (pos != std::string_view::npos) {
				++result;
				pos = str.find(findStr, pos + findStr.length()); // continue search after the findStr
			};

			return result;
		};



		auto countHex(std::string_view str)
		{
			constexpr int hexDigitCount{2};
			constexpr std::string_view hexPattern{"\\x"};

			int result{0};
			std::size_t pos{str.find(hexPattern)};

			while (pos != std::string_view::npos) {
				pos += hexPattern.size();	// pos after "\x"

				if ((pos + 1 < str.length()) && std::isxdigit(str[pos]) && std::isxdigit(str[pos + 1])) {
					++result;
					pos += hexDigitCount;	// pos after "\x##"
				}

				pos = str.find(hexPattern, pos);
			};

			return result;
		};



		auto calculateDifference(std::string_view str)
		{
			str.remove_prefix(1);
			str.remove_suffix(1);
			constexpr int min_result{2};	// +2 from first and last "
			constexpr int hexSizeDifference{3};

			return min_result + countStr("\\\\", str) + countStr("\\\"", str) + hexSizeDifference * countHex(str);
		}
	}
}



namespace Part2 { // Calculations
	static auto calculateDifference(std::string_view str)
	{
		str.remove_prefix(1);
		str.remove_suffix(1);
		constexpr int min_result{4}; // +4 from leading and trailing "

		const auto countChr([&](const char findChr) {
			return static_cast<int>(std::count(str.cbegin(), str.cend(), findChr));
		});

		return min_result + countChr('\\') + countChr('\"');
	}
}



static auto accumulateDifference(const SantasList& list, int(*fcn)(std::string_view input))
{
	return std::accumulate(list.cbegin(), list.cend(), 0, [=](int sum, std::string_view str) {
		return sum + fcn(str);
	});
};




namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(Part1::calculateDifference("\"\""), 2);
			io.printTest(Part1::calculateDifference("\"abc\""), 2);
			io.printTest(Part1::calculateDifference("\"aaa\\\"aaa\""), 3);
			io.printTest(Part1::calculateDifference("\"\\x27\""), 5);

			io.printTest(Part2::calculateDifference("\"\""), 4);
			io.printTest(Part2::calculateDifference("\"abc\""), 4);
			io.printTest(Part2::calculateDifference("\"aaa\\\"aaa\""), 6);
			io.printTest(Part2::calculateDifference("\"\\x27\""), 5);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D08 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d08}};
		testPuzzle(io);

		const auto input{io.readInputFile<std::string>()};
		validate(input);

		io.printSolution(accumulateDifference(input, Part1::calculateDifference), 1333);
		io.printSolution(accumulateDifference(input, Part2::calculateDifference), 2046);
	}
}