module;

#include <algorithm>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D01;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Calculations
	constexpr std::string_view digitString{"0123456789"};
	const std::vector<std::string_view> numberStrings{"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};


	auto findFirst(std::string_view line, bool part2)
	{
		auto pos{line.find_first_of(digitString)};
		auto digit{std::stoi(std::string{line[pos]})};

		if (part2) {
			int index{1}; // numberStrings[0] = "one"
			std::ranges::for_each(numberStrings, [&](std::string_view str) {
				const auto foundPos{line.find(str)};
				if (foundPos < pos) {
					pos = foundPos;
					digit = index;
				}
				++index;
			});
		}

		return digit;
	}



	auto findLast(std::string_view line, bool part2)
	{
		auto pos{line.find_last_of(digitString)};
		auto digit{std::stoi(std::string{line[pos]})};

		if (part2) {
			int index{1}; // numberStrings[0] = "one"
			std::ranges::for_each(numberStrings, [&](std::string_view str) {
				const auto foundPos{line.rfind(str)};
				if (foundPos != std::string_view::npos && foundPos > pos) {
					pos = foundPos;
					digit = index;
				}
				++index;
			});
		}

		return digit;
	}



	auto getCalibrationValue(std::string_view line, bool part2)
	{
		return findFirst(line, part2) * 10 + findLast(line, part2);
	}



	auto accumulateValues(const std::vector<std::string>& input, bool part2)
	{
		return std::accumulate(input.cbegin(), input.cend(), 0, [&](int sum, std::string_view i) {
			return sum + getCalibrationValue(i, part2);
		});
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(getCalibrationValue("1abc2", false), 12);
			io.printTest(getCalibrationValue("pqr3stu8vwx", false), 38);
			io.printTest(getCalibrationValue("a1b2c3d4e5f", false), 15);
			io.printTest(getCalibrationValue("treb7uchet", false), 77);

			io.printTest(getCalibrationValue("two1nine", true), 29);
			// changed input, because in part one there must be at least one (or two) digit(s), no digit will break function
			io.printTest(getCalibrationValue("eightwo3three", true), 83);
			io.printTest(getCalibrationValue("abcone2threexyz", true), 13);
			io.printTest(getCalibrationValue("xtwone3four", true), 24);
			io.printTest(getCalibrationValue("4nineeightseven2", true), 42);
			io.printTest(getCalibrationValue("zoneight234", true), 14);
			io.printTest(getCalibrationValue("7pqrstsixteen", true), 76);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D01 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d01}};
		testPuzzle(io);

		const auto calibrationDocument{io.readInputFile<std::string>()};
		io.printSolution(accumulateValues(calibrationDocument, false), 53194);
		io.printSolution(accumulateValues(calibrationDocument, true), 54249);
	}
}