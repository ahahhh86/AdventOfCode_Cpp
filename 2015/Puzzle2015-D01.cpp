module;

#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D01;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Direction
	{
		down = -1,
		up = 1,
	};



	Direction charToDirection(char c)
	{
		switch (c) {
		case '(':
			return Direction::up;
		case ')':
			return Direction::down;
		default:
			throw AOC::InvalidFileInput();
		}
	}



	auto& operator>>(std::ifstream& in, Direction& dir)
	{
		char buffer{};
		in >> buffer;
		dir = charToDirection(buffer);
		return in;
	}
}



namespace { // Calculations
	struct PuzzleResult
	{
		int floor{};
		int atBasement{};
	};



	PuzzleResult moveElevator(std::vector<Direction>&& instructions)
	{
		constexpr int groundFloor = 0;
		constexpr int basementFloor = -1;

		constexpr int indexStart = 1;
		constexpr int invalidIndex = indexStart - 1;

		PuzzleResult result{groundFloor, invalidIndex};
		int index{indexStart};

		std::ranges::for_each(instructions, [&](Direction dir) {
			result.floor += static_cast<int>(dir);

			if (result.floor == basementFloor && result.atBasement == invalidIndex) {
				result.atBasement = index;
			}
			++index;
		});

		return result;
	}
}



namespace { // Testing
	auto FromSV(std::string_view str)
	{
		std::vector<Direction> result{};
		result.reserve(str.size());

		std::ranges::for_each(str, [&](char c) {
			result.push_back(charToDirection(c));
		});

		return result;
	}



	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(moveElevator(FromSV("(())")).floor, 0);
			io.printTest(moveElevator(FromSV("()()")).floor, 0);
			io.printTest(moveElevator(FromSV("(((")).floor, 3);
			io.printTest(moveElevator(FromSV("(()(()(")).floor, 3);
			io.printTest(moveElevator(FromSV("))(((((")).floor, 3);
			io.printTest(moveElevator(FromSV("())")).floor, -1);
			io.printTest(moveElevator(FromSV("))(")).floor, -1);
			io.printTest(moveElevator(FromSV(")))")).floor, -3);
			io.printTest(moveElevator(FromSV(")())())")).floor, -3);

			io.printTest(moveElevator(FromSV(")")).atBasement, 1);
			io.printTest(moveElevator(FromSV("()())")).atBasement, 5);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D01 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d01}};
		testPuzzle(io);

		const auto result{moveElevator(io.readInputFile<Direction>())};
		io.printSolution(result.floor, 280);
		io.printSolution(result.atBasement, 1797);
	}
}