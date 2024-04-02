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
import Grid2D;		// TODO: make own Grid class, inherited from Grid2D



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



	// do not use recursive function: cases call stack to overflow
	//void makeTilePlain(Grid& grid, const Point& pos)
	//{
	//	if (!pos.isInBounds({0, 0}, {gridSize - 1, gridSize - 1})) { return; }
	//	if (grid[pos] != Tile::Unknown) { return; }

	//	grid[pos] = Tile::Plain;
	//	makeTilePlain(grid, pos + directionToPoint(Direction::Up));
	//	makeTilePlain(grid, pos + directionToPoint(Direction::Right));
	//	makeTilePlain(grid, pos + directionToPoint(Direction::Down));
	//	makeTilePlain(grid, pos + directionToPoint(Direction::Left));
	//}



	void makeBorderTilePlain(Grid& grid, const Point& pos)
	{
		Tile& tile{grid[pos]};
		if (tile == Tile::Unknown) { tile = Tile::Plain; }
	}



	void makeBorderPlain(Grid& grid)
	{
		constexpr auto maxGrid{gridSize - 1};
		for (int i {0}; i < gridSize; ++i) {
			makeBorderTilePlain(grid, {0, i});
			makeBorderTilePlain(grid, {maxGrid, i});
		};

		for (int i{1}; i < maxGrid; ++i) {
			makeBorderTilePlain(grid, {i, 0});
			makeBorderTilePlain(grid, {i, maxGrid});
		};
	}



	bool isAdjacentPlain(Grid& grid, const Point& pos)
	{
		// TODO: maybe Check bounds
		return grid[pos + directionToPoint(Direction::Up)] == Tile::Plain
			|| grid[pos + directionToPoint(Direction::Right)] == Tile::Plain
			|| grid[pos + directionToPoint(Direction::Down)] == Tile::Plain
			|| grid[pos + directionToPoint(Direction::Left)] == Tile::Plain;

	}



	auto makeInnerPlain(Grid& grid)
	{
		constexpr auto maxGrid{gridSize - 1};
		int count{0};

		for (int i{1}; i < maxGrid; ++i) {
			for (int j{1}; j < maxGrid; ++j) {
				const Point pos{i, j};
				if (grid[pos] == Tile::Unknown && isAdjacentPlain(grid, pos)) {
					grid[pos] = Tile::Plain;
					++count;
				}
			}
		}

		return count;
	}



	void makePlain(Grid& grid)
	{
		makeBorderPlain(grid);

		while (makeInnerPlain(grid)) {
			// do nothing, just loop condition
		}
	}



	auto countHoles(Grid& grid)
	{
		return static_cast<int>(std::count_if(grid.cbegin(), grid.cend(), [&](Tile i){
			return i == Tile::Hole;
		}));
	}



	auto countHoles2(Grid& grid) // TODO: maybe make all Unknown Hole before counting
	{
		return static_cast<int>(std::count_if(grid.cbegin(), grid.cend(), [&](Tile i) {
			return i == Tile::Hole || i == Tile::Unknown;
		}));
	}



	// TODO: Part2
	// numbers are too big, need another way to calculate the numbers
	auto intToDirection(int value)
	{
		switch (value) {
			case 0: return Direction::Right;
			case 1: return Direction::Down;
			case 2: return Direction::Left;
			case 3: return Direction::Up;
		default:
			throw AOC::InvalidInputData("intToDirection()");
		}
	}



	auto reinterpretInstructions(const InstructionVector& instructions)
	{
		InstructionVector result{};

		std::ranges::for_each(instructions, [&](const Instruction& i){
			Instruction buffer{};
			buffer.direction = intToDirection(i.color % 16);
			buffer.length = i.color / 16;
			result.push_back(buffer);
		});

		return result;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const InstructionVector instructions{
				readInstruction(std::stringstream{"R 6 (#70c710)"}),
				readInstruction(std::stringstream{"D 5 (#0dc571)"}),
				readInstruction(std::stringstream{"L 2 (#5713f0)"}),
				readInstruction(std::stringstream{"D 2 (#d2c081)"}),
				readInstruction(std::stringstream{"R 2 (#59c680)"}),
				readInstruction(std::stringstream{"D 2 (#411b91)"}),
				readInstruction(std::stringstream{"L 5 (#8ceee2)"}),
				readInstruction(std::stringstream{"U 2 (#caa173)"}),
				readInstruction(std::stringstream{"L 1 (#1b58a2)"}),
				readInstruction(std::stringstream{"U 2 (#caa171)"}),
				readInstruction(std::stringstream{"R 2 (#7807d2)"}),
				readInstruction(std::stringstream{"U 3 (#a77fa3)"}),
				readInstruction(std::stringstream{"L 2 (#015232)"}),
				readInstruction(std::stringstream{"U 2 (#7a21e3)"}),
			};

			io.startTests();

			auto grid{digFrame(instructions)};
			makePlain(grid);

			io.printTest(countHoles(grid), 38);
			io.printTest(countHoles2(grid), 62);

			auto newInstructions{reinterpretInstructions(instructions)};

			io.printTest(newInstructions[0].length, 461937);
			io.printTest(newInstructions[0].direction == Direction::Right, true);

			io.printTest(newInstructions[1].length, 56407);
			io.printTest(newInstructions[1].direction == Direction::Down, true);

			io.printTest(newInstructions[2].length, 356671);
			io.printTest(newInstructions[2].direction == Direction::Right, true);

			io.printTest(newInstructions[6].length, 577262);
			io.printTest(newInstructions[6].direction == Direction::Left, true);

			io.printTest(newInstructions[7].length, 829975);
			io.printTest(newInstructions[7].direction == Direction::Up, true);

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
		makePlain(grid);

		io.printSolution(countHoles2(grid), 76387);
		// TODO: Part2
	}
}