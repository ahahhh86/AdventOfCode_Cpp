module;

#include <fstream>
#include <iostream>

module Puzzle2015:D20;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	using uint_t = unsigned int;



	// WARNING: takes (on my computer) about 10 min. to find the right solution
	// TODO: How to make faster
	uint_t calculatePresents(unsigned int houseIndex)
	{
		constexpr uint_t multiplicator{10};			// to save some operations, just do this once at the end
		const uint_t loop_end{houseIndex / 2};		// elves between houseIndex/2 and houseIndex will not stop here
		uint_t result = 1 + houseIndex;				// elves 1 and houseIndex will always stop here

		for (uint_t i{2}; i <= loop_end; ++i) {
			if (houseIndex % i == 0) {
				result += i;
			}
		}

		return result * multiplicator;
	}



	uint_t calculatePresentsPart2(unsigned int houseIndex)
	{
		constexpr uint_t max_presents_per_elf{50};
		constexpr uint_t multiplicator{11};							// to save some operations, just do this once at the end
		const uint_t loop_end{houseIndex / max_presents_per_elf};
		uint_t result = houseIndex;									// Assume houseIndex > 50, skip first house

		for (uint_t i{houseIndex / 2}; i >= loop_end; --i) {
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

		uint_t input{};
		io.getInputFile() >> input;
		if (input == 0) throw AOC::InvalidFileInput();

		uint_t houseIndex{2};
		//uint_t houseIndex {665270}; // get headstart for debugging
		while (calculatePresents(houseIndex) <= input) {
			++houseIndex;
		}
		io.printSolution(houseIndex, 665280U);

		while (calculatePresentsPart2(houseIndex) <= input) {
			++houseIndex;
		}
		io.printSolution(houseIndex, 705600U);
	}
}