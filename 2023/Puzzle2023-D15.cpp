module;

#include <algorithm>
#include <fstream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D15;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *
// TODO: Part 2

namespace { // Input
	auto splitStr(std::string str, std::string_view delimiter)
	{
		std::vector<std::string> result{};

		auto end {str.find(delimiter)};

		while (end != std::string::npos) {
			const auto buffer{str.substr(0, end)};

			result.push_back(buffer);
			str.erase(str.begin(), std::next(str.begin(), static_cast<long long>(end) + 1));

			end = str.find(delimiter);
		}
		result.push_back(str.substr(0, end));

		return result;
	}
}



namespace { // Calculations
	auto hashChar(char c, int currentValue)
	{
		int i{c};
		currentValue += i;
		currentValue *= 17;
		currentValue %= 256;

		return currentValue;
	}



	auto hash(std::string_view str)
	{
		return std::accumulate(str.cbegin(), str.cend(), 0, [&](int sum, char c) {
			return hashChar(c, sum);
		});
	}



	auto accumulateHashes(const std::vector<std::string>& strs)
	{
		return std::accumulate(strs.cbegin(), strs.cend(), 0, [&](int sum, std::string_view str) {
			return sum + hash(str);
		});
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const auto initializationSequence{splitStr("rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7", ",")};

			io.startTests();

			io.printTest(hash(initializationSequence[0]), 30);
			io.printTest(hash(initializationSequence[1]), 253);
			io.printTest(hash(initializationSequence[2]), 97);
			io.printTest(hash(initializationSequence[3]), 47);
			io.printTest(hash(initializationSequence[4]), 14);
			io.printTest(hash(initializationSequence[5]), 180);
			io.printTest(hash(initializationSequence[6]), 9);
			io.printTest(hash(initializationSequence[7]), 197);
			io.printTest(hash(initializationSequence[8]), 48);
			io.printTest(hash(initializationSequence[9]), 214);
			io.printTest(hash(initializationSequence[10]), 231);

			io.printTest(accumulateHashes(initializationSequence), 1320);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D15 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d15}};
		testPuzzle(io);

		const auto initializationSequence{splitStr(io.readInputFile<std::string>()[0], ",")};
		io.printSolution(accumulateHashes(initializationSequence), 495972);
		//io.printSolution(accumulateGearRatios(schematic), 86879020);
	}
}