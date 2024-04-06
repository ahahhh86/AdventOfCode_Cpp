module;

#include <algorithm>
#include <array>
//#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D21;

import BasicImports;
import Grid2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Plot
	{
		Start,
		Garden,
		Rock,
	};
	using Grid = AOC::Grid2D<Plot>;
	using Point = AOC::Grid2D<Plot>::Point;

	constexpr Point invalidPos{-1, -1};

	struct InputData
	{
		Grid grid;
		Point start;
	};

	auto readGrid(std::vector<std::string>&& in)
	{
		std::ranges::for_each(in, [&](std::string_view str) {
			if (str.size() != in[0].size()) { throw AOC::InvalidInputData{"readGrid(): dimensions do not match"}; }
		});

		const int width{static_cast<int>(in[0].size())};
		const int height{static_cast<int>(in.size())};

		InputData result{{{width, height}}, {invalidPos}};
		for (int i{0}; i < width; ++i) {
			for (int j{0}; j < height; ++j) {
				switch (in[j][i]) {
				case 'S':
					result.start = {i, j};
					[[fallthrough]];
				case '.':
					result.grid[{i, j}] = Plot::Garden;
					break;
				case'#':
					result.grid[{i, j}] = Plot::Rock;
					break;
				default:
					throw AOC::InvalidInputData{"readGrid(): invalid plot"};
				}
			}
		}

		if (result.start == invalidPos) { throw AOC::InvalidInputData{"readGrid(): no start position"}; }
		return result;
	}
}



namespace { // Calculations
	using Positions = std::set<Point>;

	namespace Part1 {
		static bool isValidPos(Point pos, const Grid& grid)
		{
			const Point gridSize{grid.getGridSize()};
			if (!pos.isInBounds({0, 0}, {gridSize.x - 1,gridSize.y - 1})) { return false; }


			return grid[pos] == Plot::Garden;
		}
	}

	namespace Part2 {
		static int recalculatePosition(int pos, int bound)
		{
			if (pos < 0) {
				while (pos < 0) { // TODO: find formula
					pos += bound;
				}
				return pos;
			} else {
				return pos % bound;
			}
		}

		static bool isValidPos(Point pos, const Grid& grid)
		{
			const Point gridSize{grid.getGridSize()};
			const auto org{pos};
			
			pos.x = recalculatePosition(pos.x, gridSize.x);
			pos.y = recalculatePosition(pos.y, gridSize.y);

			if (!pos.isInBounds({0, 0}, {gridSize.x - 1,gridSize.y - 1})) {
				throw AOC::aocError{std::to_string(pos.x) + "|" + std::to_string(pos.y)
					+ "-" + std::to_string(org.x) + "|" + std::to_string(org.y)};
			}
			return grid[pos] == Plot::Garden;
		}
	}



	using ValidateFcn = bool(*)(Point pos, const Grid& grid);
	auto calculateNextStep(const Positions& pos, const Grid& grid, ValidateFcn fcn)
	{
		constexpr std::array steps{
			Point{-1, 0},
			Point{0, 1},
			Point{1, 0},
			Point{0, -1},
		};

		Positions result{};
		std::ranges::for_each(pos, [&](const Point& p){
			std::ranges::for_each(steps, [&](const Point& step) {
				const auto newPos{p + step};
				if (fcn(newPos, grid)) {
					result.insert(newPos);
				}
			});
		});

		return result;
	}



	auto calculateSteps(const Point& start, const Grid& grid, ValidateFcn fcn, int count = 64)
	{
		Positions positions{start};

		for (int i{0}; i < count; ++i) {
			positions = calculateNextStep(positions, grid, fcn);
		}

		return static_cast<int>(positions.size());
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			InputData test{readGrid({
				"...........",
				".....###.#.",
				".###.##..#.",
				"..#.#...#..",
				"....#.#....",
				".##..S####.",
				".##..#...#.",
				".......##..",
				".##.#.####.",
				".##..##.##.",
				"...........",
			})};

			io.startTests();

			Positions pos{test.start};
			pos = calculateNextStep(pos, test.grid, Part1::isValidPos);
			io.printTest(pos.size(), 2ULL);

			pos = calculateNextStep(pos, test.grid, Part1::isValidPos);
			io.printTest(pos.size(), 4ULL);

			pos = calculateNextStep(pos, test.grid, Part1::isValidPos);
			io.printTest(pos.size(), 6ULL);

			io.printTest(calculateSteps(test.start, test.grid, Part1::isValidPos, 6), 16);

			// Part 2
			io.printTest(Part2::recalculatePosition(0, 11), 0);
			io.printTest(Part2::recalculatePosition(11, 11), 0);
			io.printTest(Part2::recalculatePosition(1, 11), 1);
			io.printTest(Part2::recalculatePosition(12, 11), 1);
			io.printTest(Part2::recalculatePosition(-1, 11), 10);
			io.printTest(Part2::recalculatePosition(-12, 11), 10);

			io.printTest(calculateSteps(test.start, test.grid, Part2::isValidPos, 10), 50);
			io.printTest(calculateSteps(test.start, test.grid, Part2::isValidPos, 50), 1594);
			//io.printTest(calculateSteps(test.start, test.grid, Part2::isValidPos, 100), 6536);
			//io.printTest(calculateSteps(test.start, test.grid, Part2::isValidPos, 500), 167004); // Takes too long
			//io.printTest(calculateSteps(test.start, test.grid, Part2::isValidPos, 1000), 668697);
			//io.printTest(calculateSteps(test.start, test.grid, Part2::isValidPos, 5000), 16733044);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D21 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d21}};
		testPuzzle(io);

		const auto garden{readGrid(io.readInputFile<std::string>())};
		io.printSolution(calculateSteps(garden.start, garden.grid, Part1::isValidPos), 3737);
		//io.printSolution(calculateSteps(garden.start, garden.grid, Part2::isValidPos), 0);// TODO: takes too long
	}
}