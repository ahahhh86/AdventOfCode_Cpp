module;

#include <limits>
#include <string>
#include <string_view>
#include "md5.h"

module Puzzle2015:D04;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Calculations
	using Index = int;

	constexpr std::string_view part1StartsWith{"00000"};
	constexpr std::string_view part2StartsWith{"000000"};

	Index findHash(std::string_view secretKey, std::string_view hashStart, Index startIndex = 0)
	{
		constexpr auto loopEnd{std::numeric_limits<Index>::max()};

		for (Index i{startIndex}; i <= loopEnd; ++i) { // potentially long loop, but not infinite
			const std::string hash = md5(secretKey.data() + std::to_string(i));
			if (hash.starts_with(hashStart)) return i;
		}

		throw AOC::aocError{"No matching hash found!"};
//		return -1;
	}
}



namespace { // Tests
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(findHash("abcdef", part1StartsWith), 609043);
			io.printTest(findHash("pqrstuv", part1StartsWith), 1048970);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D04 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d04}};
		testPuzzle(io);

		const std::string secretKey{io.readInputFile<std::string>()[0]};
		const Index part1{findHash(secretKey, part1StartsWith)};
		io.printSolution(part1, 254575);

		// the hash of part 2 starts with one "0" more, so every number that did not work
		// with part 1 can be skipped
		io.printSolution(findHash(secretKey, part2StartsWith, part1), 1038736);
	}
}