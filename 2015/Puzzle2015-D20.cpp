module;

#include <fstream>
#include <iostream>

module Puzzle2015:D20;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	auto readMaxPresents(std::ifstream&& in)
	{
		int buffer{};
		in >> buffer;
		if (buffer == 0) throw AOC::InvalidFileInput();
		return buffer;
	}
}



namespace Part1 { // Calculations
	// WARNING: takes (on my computer) about 10 min. to find the right solution
	// TODO: How to make faster
	static auto calculatePresents(int houseIndex)
	{
		constexpr int multiplicator{10};		// to save some operations, just do this once at the end
		const int loop_end{houseIndex / 2};		// elves between houseIndex/2 and houseIndex will not stop here
		int result {1 + houseIndex};			// elves 1 and houseIndex will always stop here

		for (int i{2}; i <= loop_end; ++i) {
			if (houseIndex % i == 0) {
				result += i;
			}
		}

		return result * multiplicator;
	}
}



namespace Part2 { // Calculations
	static auto calculatePresents(int houseIndex)
	{
		constexpr int max_presents_per_elf{50};
		constexpr int multiplicator{11};							// to save some operations, just do this once at the end
		const int loop_end{houseIndex / max_presents_per_elf};
		int result {houseIndex};									// Assume houseIndex > 50, skip first house

		for (int i{houseIndex / 2}; i >= loop_end; --i) {
			if (houseIndex % i == 0) {
				result += i;
			}
		}

		return result * multiplicator;
	}
}



namespace { // Testing
	// TODO:
	//void testPuzzle(AOC::IO& io)
	//{
	//	if (AOC::debugMode) {
	//		const StatVector stats{
	//			{14, 10, 127},
	//			{16, 11, 162},
	//		};

	//		io.startTests();

	//		const auto raceResult{getRaceResults(stats, 1000)};
	//		io.printTest(raceResult.first, 1120);
	//		io.printTest(raceResult.second, 689);

	//		io.endTests();
	//	}
	//}
}



namespace AOC::Y2015::D20 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d20}};
		//testPuzzle(io);

		const int input{readMaxPresents(io.getInputFile())};

		//int houseIndex{2};
		int houseIndex {665270}; // get headstart for debugging
		while (Part1::calculatePresents(houseIndex) <= input) {
			++houseIndex;
		}
		io.printSolution(houseIndex, 665280);

		while (Part2::calculatePresents(houseIndex) <= input) {
			++houseIndex;
		}
		io.printSolution(houseIndex, 705600);
	}
}