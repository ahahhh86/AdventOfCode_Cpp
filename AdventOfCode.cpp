/*
	This is my soution for the puzzles of https://adventofcode.com/
	I build this with Mirosoft Visual Studio 2022 with /std:c++latest enabled
	and modules enabled, so may not be easily ported to other systems.
*/

// TODO: add Command line argument support
// Puzzle description -> notepad++ find: (?<=.{80})\s replace: $0\n

#include <iostream>
#include <string>

//import std; // TODO: import if IntelliSense works with it
import SolvePuzzle;
import IO;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	void solveAOC()
	{
		using namespace AOC;

		constexpr Date startDate{Year::y2015, Day::d01};
		constexpr Date maxDate{Year::y2016, Day::d01};

		for (Date i{startDate}; i < maxDate; ++i) {
			solvePuzzle(i);
		}
	}
}



int main()
{
	using namespace AOC;

	std::cout << "* * * Advent of Code  * * * * * * * * *\n\n";

	//if (debugMode) {
		solvePuzzle({Year::y2023, Day::d13});
	//} else {
		//solveAOC();
	//}

	std::cout << "* * * * * * * * * * * * * * * * * * * *\n\n\n";
#ifdef NDEBUG
	// Window should not close after completing
	// normally handled by Visual Studio, but not in finished *.exe
	std::string buffer{};
	std::getline(std::cin, buffer);
#endif // NDEBUG
}