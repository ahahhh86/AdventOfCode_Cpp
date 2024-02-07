module;

//#include <algorithm>
//#include <numeric>
//#include <string>
//#include <string_view>
//#include <vector>

module Puzzle2023:D17;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
}



namespace { // Calculations
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			//io.printTest(accumulateEngineParts(testSchematic), 4361);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D17 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d17}};
		testPuzzle(io);

		//const auto schematic{readSchematic(io.readInputFile<std::string>())};
		//io.printSolution(accumulateEngineParts(schematic), 540131);
		//io.printSolution(accumulateGearRatios(schematic), 86879020);
	}
}