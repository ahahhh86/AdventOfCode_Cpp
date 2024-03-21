module;

#include <algorithm>
#include <fstream>
#include <set>
#include <string_view>
#include <vector>

module Puzzle2015:D03;

import BasicImports;
import Point2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Direction
	{
		north,
		east,
		south,
		west,
	};



	Direction charToDirection(char c)
	{
		switch (c) {
		case '^':
			return Direction::north;
		case '>':
			return Direction::east;
		case 'v':
			return Direction::south;
		case '<':
			return Direction::west;
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
	using Point = AOC::Point2D<int>;

	class Character: public Point
	{
	public:
		void move(Direction dir);
	};



	void Character::move(Direction dir)
	{
		switch (dir) {
		case Direction::north:
			++y;
			break;
		case Direction::east:
			++x;
			break;
		case Direction::south:
			--y;
			break;
		case Direction::west:
			--x;
			break;
		}
	}



	using Instructions = std::vector<Direction>;

	auto moveAlong(const Instructions& in, bool withRobot)
	{
		constexpr Point startPosition{0, 0};

		std::set<Point> result{startPosition};

		Character santa{startPosition};
		Character robot{startPosition};
		bool isSanta{true};				// switch between Santa and Robot (if necessary)

		std::ranges::for_each(in, [&](Direction dir) {
			Character& character{isSanta ? santa : robot};

			character.move(dir);
			result.insert(character);

			if (withRobot) isSanta = !isSanta;
		});

		return result;
	}



	auto countPresents(const Instructions& in, bool withRobot)
	{
		const auto visitedHouses{moveAlong(in, withRobot)};
		return static_cast<int>(std::size(visitedHouses));
	}
}



namespace { // Testing
	auto FromSV(std::string_view str)
	{
		Instructions result{};
		result.reserve(str.size());

		std::ranges::for_each(str, [&](char c) {
			result.emplace_back(charToDirection(c));
		});

		return result;
	}



	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(countPresents(FromSV(">"), false), 2);
			io.printTest(countPresents(FromSV("^>v<"), false), 4);
			io.printTest(countPresents(FromSV("^v^v^v^v^v"), false), 2);

			io.printTest(countPresents(FromSV("^v"), true), 3);
			io.printTest(countPresents(FromSV("^>v<"), true), 3);
			io.printTest(countPresents(FromSV("^v^v^v^v^v"), true), 11);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D03 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d03}};
		testPuzzle(io);

		const auto inputData{io.readInputFile<Direction>()};
		io.printSolution(countPresents(inputData, false), 2565);
		io.printSolution(countPresents(inputData, true), 2639);
	}
}