module;

#include <algorithm>
#include <fstream>
#include <numeric>
#include <sstream>
#include <string>
//#include <string_view>
#include <vector>

module Puzzle2023:D18;

import BasicImports;
import Grid2D;		// TODO: without Grid, use only vector with outline, is it possible?



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Direction
	{
		Up,
		Right,
		Down,
		Left,
	};



	auto& operator>>(std::stringstream& in, Direction& dir)
	{
		char buffer{};
		in >> buffer;

		switch (buffer) {
		case 'U':
			dir = Direction::Up;
			break;
		case 'R':
			dir = Direction::Right;
			break;
		case 'D':
			dir = Direction::Down;
			break;
		case 'L':
			dir = Direction::Left;
			break;

		default:
			throw AOC::InvalidFileInput();
		}

		return in;
	}



	struct Instruction
	{
		Direction direction{Direction::Up};
		int length{0};
		int color{0};
	};
	using InstructionVector = std::vector<Instruction>;



	auto readInstruction(std::stringstream&& in)
	{
		// format example: L 9 (#0e8fc0)
		Instruction result{};

		in >> result.direction;
		in >> result.length;

		std::string buffer{};
		in >> buffer;
		buffer.pop_back();
		buffer.replace(0, 2, "0X");
		try {
			result.color = std::stoi(buffer, nullptr, 16);
		} catch (...) {
			throw AOC::InvalidFileInput();
		}

		if (in.fail()) { throw AOC::InvalidFileInput(); }

		return result;
	}



	auto& operator>>(std::ifstream& in, Instruction& instruction)
	{
		std::string buffer{};
		std::getline(in, buffer);
		instruction = readInstruction(std::stringstream{buffer});

		return in;
	}
}



namespace { // Calculations
	enum class Tile
	{
		Unknown,
		Hole,
		Plain,
	};

	using Grid = AOC::Grid2D<Tile>;
	using Point = AOC::Point2D<int>;
	constexpr int gridSize{1000};			// adjust size, if out of bounds


	Point directionToPoint(Direction d)
	{
		switch (d) {
		case Direction::Up:
			return {-1, 0};
		case Direction::Right:
			return {0, 1};
		case Direction::Down:
			return {1, 0};
		case Direction::Left:
			return {0, -1};
		default:
			throw AOC::InvalidInputData("directionToPoint()");
		}
	}



	//auto createGrid(int size)
	//{
	//	Grid result{{size, size}};
	//	result.forSub({0, 0}, {size - 1, size - 1}, [&](Tile& i) {
	//		i = Tile::Unknown;
	//	});
	//	return result;
	//}



	void digLine(Grid& grid, Point& pos, const Point& dir, int length)
	{
		for (int i{0}; i < length; ++i) {
			pos += dir;
			grid[pos] = Tile::Hole;
		}
	}



	auto digFrame(const InstructionVector& instructions)
	{
		constexpr Point start{gridSize/2, gridSize/2};
		Grid result{{gridSize, gridSize}};
		result[start] = Tile::Hole;
		auto pos{start};

		std::ranges::for_each(instructions, [&](const Instruction& i) {
			digLine(result, pos, directionToPoint(i.direction), i.length);
		});

		if (pos != start) { throw AOC::InvalidInputData("digFrame()"); }

		return result;
	}



	void makeTilePlain(Grid& grid, const Point& pos)
	{
		if (!pos.isInBounds({0, 0}, {gridSize - 1, gridSize - 1})) { return; }
		if (grid[pos] != Tile::Unknown) { return; }

		grid[pos] = Tile::Plain;
		makeTilePlain(grid, pos + directionToPoint(Direction::Up));
		makeTilePlain(grid, pos + directionToPoint(Direction::Right));
		makeTilePlain(grid, pos + directionToPoint(Direction::Down));
		makeTilePlain(grid, pos + directionToPoint(Direction::Left));
	}



	void makeFramePlain(Grid& grid)
	{
		/*grid.forSub({0, 0}, {0, gridSize - 1}, [&](Tile& i) {
			if (i == Tile::Unknown) { i = Tile::Plain; }
		});*/
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			//io.printTest(0, 0);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D18 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d18}};
		testPuzzle(io);

		const auto instructions{io.readInputFile<Instruction>()};
		auto grid{digFrame(instructions)};

		int sum{0};
		for (auto i{grid.cbegin()}; i < grid.cend(); ++i) {
			sum += static_cast<int>(*i);
		}

		io.printSolution(sum, 4696);
	}
}