module;

#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D05;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace Part1 { // Calculations
	namespace {
		bool has3vowels(std::string_view str)
		{
			constexpr int minVowelCount{3};
			int vowelCount{0};
			for (char c : str) {
				switch (c) {
				case 'a':
				case 'e':
				case 'i':
				case 'o':
				case 'u':
					++vowelCount;
				}

				if (vowelCount == minVowelCount) {
					return true;
				}
			}

			return false;
		}



		bool hasDoubleLetter(std::string_view str, int spaceBetween = 0)
		{
			const int offset{spaceBetween + 1};

			for (auto c{std::next(str.cbegin(), offset)}; c != str.cend(); ++c) {
				if (*std::prev(c, offset) == *c) return true;
			}

			return false;
		}



		bool hasNaughtySubstring(std::string_view str)
		{
			return str.contains("ab")
				|| str.contains("cd")
				|| str.contains("pq")
				|| str.contains("xy");
		}



		bool isNice(std::string_view str)
		{
			return has3vowels(str) && hasDoubleLetter(str) && !hasNaughtySubstring(str);
		}
	}
}



namespace Part2 { // Calculations
	namespace {
		bool hasDoubleSequence(std::string_view str)
		{
			constexpr std::size_t sequenceLength{2};
			const std::size_t loopEnd{str.length() - 2 * sequenceLength + 1};

			for (std::size_t i{0}; i < loopEnd; ++i) {
				const std::string_view sequence{str.substr(i, sequenceLength)};

				if (str.find(sequence, i + sequenceLength) != std::string_view::npos) {
					return true;
				}
			}

			return false;
		}



		bool isNice(std::string_view str)
		{
			return Part1::hasDoubleLetter(str, 1) && hasDoubleSequence(str);
		}
	}
}




namespace { // Calculations
	auto countNice(const std::vector<std::string>& list, bool(*fcn)(std::string_view))
	{
		const auto result{std::ranges::count_if(list, [&](std::string_view str) {
			return fcn(str);
		})};
		return static_cast<int>(result);
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(Part1::isNice("ugknbfddgicrmopn"), true);
			io.printTest(Part1::isNice("aaa"), true);
			io.printTest(Part1::isNice("jchzalrnumimnmhp"), false);
			io.printTest(Part1::isNice("haegwjzuvuyypxyu"), false);
			io.printTest(Part1::isNice("dvszwmarrgswjxmb"), false);

			io.printTest(Part2::isNice("qjhvhtzxzqqjkmpb"), true);
			io.printTest(Part2::isNice("xxyxx"), true);
			io.printTest(Part2::isNice("uurcxstgmygtbstg"), false);
			io.printTest(Part2::isNice("ieodomkazucvgmuy"), false);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D05 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d05}};
		testPuzzle(io);

		const auto santasList{io.readInputFile<std::string>()};
		io.printSolution(countNice(santasList, Part1::isNice), 258);
		io.printSolution(countNice(santasList, Part2::isNice), 53);
	}
}