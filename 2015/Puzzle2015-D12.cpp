module;

#include <string>

module Puzzle2015:D12;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Calculations
	auto sumNumbers(std::string_view str)
	{
		int result{0};

		// Assuming input does not end with a number, should end with closed brace
		for (auto numberStart{str.cbegin()}; numberStart != str.cend(); ++numberStart) {
			if (std::isdigit(*numberStart) || *numberStart == '-') {
				auto numberEnd{std::next(numberStart)};
				while (std::isdigit(*numberEnd)) {
					++numberEnd;
				}
				result += std::stoi(std::string{numberStart, numberEnd});
				numberStart = numberEnd;
			}
		}

		return result;
	}



	auto findOpenBrace(std::string_view str, std::size_t index)
	{
		std::size_t posOpenBrace{};
		std::size_t posCloseBrace{};
		int count{1};

		do {
			posOpenBrace = str.find_last_of('{', index);
			posCloseBrace = str.find_last_of('}', index);

			if ((posOpenBrace < posCloseBrace) && (posCloseBrace != std::string_view::npos)) {
				++count;
				index = posCloseBrace - 1;
			} else {
				--count;
				index = posOpenBrace - 1;
			}
		} while (count > 0);

		return posOpenBrace;
	}



	auto findCloseBrace(std::string_view str, std::size_t index)
	{
		std::size_t posOpenBrace{};
		std::size_t posCloseBrace{};
		int count{1};

		do {
			posOpenBrace = str.find('{', index);
			posCloseBrace = str.find('}', index);

			if ((posOpenBrace < posCloseBrace) && (posOpenBrace != std::string_view::npos)) {
				++count;
				index = posOpenBrace + 1;
			} else {
				--count;
				index = posCloseBrace + 1;
			}
		} while (count > 0);

		return posCloseBrace;
	}



	std::string removeRed(std::string_view str)
	{
		constexpr std::string_view red{":\"red\""};

		std::string result{str};
		auto posRed{str.find(red)};
		std::size_t posOpenBrace{};
		std::size_t posCloseBrace{};

		while (posRed != std::string_view::npos) {
			posOpenBrace = findOpenBrace(result, posRed);
			posCloseBrace = findCloseBrace(result, posRed);
			result = result.erase(posOpenBrace, posCloseBrace - posOpenBrace + 1);

			posRed = result.find(red);
		}

		return result;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(sumNumbers("[1,2,3]"), 6);
			io.printTest(sumNumbers("{\"a\":2,\"b\":4}"), 6);
			io.printTest(sumNumbers("[[[3]]]"), 3);
			io.printTest(sumNumbers("{\"a\":{\"b\":4},\"c\":-1}"), 3);
			io.printTest(sumNumbers("{\"a\":[-1,1]}"), 0);
			io.printTest(sumNumbers("[-1,{\"a\":1}]"), 0);
			io.printTest(sumNumbers("[]"), 0);
			io.printTest(sumNumbers("{}"), 0);

			io.printTest(sumNumbers(removeRed("[1,2,3]")), 6);
			io.printTest(sumNumbers(removeRed("[1,{\"c\":\"red\",\"b\":2},3]")), 4);
			io.printTest(sumNumbers(removeRed("{\"d\":\"red\",\"e\":[1,2,3,4],\"f\":5}")), 0);
			io.printTest(sumNumbers(removeRed("[1,\"red\",5]")), 6);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D12 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d12}};
		testPuzzle(io);

		const auto input{io.readInputFile<std::string>()[0]};
		io.printSolution(sumNumbers(input), 156366);
		io.printSolution(sumNumbers(removeRed(input)), 96852);
	}
}