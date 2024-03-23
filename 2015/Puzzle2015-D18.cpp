module;

#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>

module Puzzle2015:D18;

import BasicImports;
import Grid2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using LightGrid = AOC::Grid2D<int>; // bool would be sufficient, but the underling std::vector works different with bool
	using Point = LightGrid::Point;
	constexpr Point lightGridSize{100, 100};



	LightGrid readLightGrid(std::ifstream&& in)
	{
		LightGrid result{lightGridSize};
		auto pos{result.begin()};

		char buffer{};
		in >> buffer;

		while (!in.eof()) {
			switch (buffer) {
			case '.':
				*pos = 0;
				++pos;
				break;

			case '#':
				*pos = 1;
				++pos;
				break;

			case '\r':
			case '\n':
				// do nothing
				break;

			default:
				throw AOC::InvalidFileInput();
			}

			in >> buffer;
		}

		if (pos != result.end()) { throw AOC::InvalidInputData("readLightGrid(): file size does not match grid size."); }

		return result;
	}
}



namespace { // Calculations
	int countAdjacentLights(const LightGrid& lg, Point pos)
	{
		int result {lg[pos] ? -1 : 0}; // if middle is alive, reduce by one, else it must have been excluded from loop
		const Point gridSize{lg.getGridSize()};

		for (int i{-1}; i < 2; ++i) {
			for (int j{-1}; j < 2; ++j) {
				const Point newPos{pos.x + i, pos.y + j};

				if (!newPos.isInBounds({0, 0}, {gridSize.x - 1, gridSize.y - 1})) { continue; }

				if (lg[newPos] != 0) {
					++result;
				}
			}
		}

		return result;
	}



	void setNextAnimation(LightGrid& lg)
	{
		// ok to create a new grid every iteration, as in release build it takes about 25 ms to solve both puzzles
		LightGrid newLights{lg};
		const Point gridSize{lg.getGridSize()};

		for (int i{0}; i < gridSize.x; ++i) {
			for (int j{0}; j < gridSize.y; ++j) {
				const int numAdjacentLights = countAdjacentLights(lg, {i, j});
				int& currentLight = newLights[{i, j}]; // just for readability

				if (lg[{i, j}] != 0) {
					if ((numAdjacentLights == 2) || (numAdjacentLights == 3)) { // stay alive
						currentLight = 1;
					} else { // die
						currentLight = 0;
					}
				} else {
					if (numAdjacentLights == 3) { // be born
						currentLight = 1;
					} else { // stay dead
						currentLight = 0;
					}
				}
			}
		}

		lg = newLights;
	}



	int accumulateLights(const LightGrid& lg)
	{
		return std::accumulate(lg.cbegin(), lg.cend(), 0);
	}



	void turnOnEdges(LightGrid& lg)
	{
		const Point gridSize{lg.getGridSize()};

		lg[{0, 0}] = 1;
		lg[{0, gridSize.y - 1}] = 1;
		lg[{gridSize.x - 1, 0}] = 1;
		lg[{gridSize.x - 1, gridSize.y - 1}] = 1;
	}



	int animateLights(LightGrid lg/*copy(!)*/, int animationCount, bool Part2 = false)
	{
		if (Part2) { turnOnEdges(lg); }

		for (int i{0}; i < animationCount; ++i) {
			setNextAnimation(lg);
			if (Part2) { turnOnEdges(lg); }
		}

		return accumulateLights(lg);
	}
}



namespace { // Testing
	auto createLG(std::vector<std::vector<char>>&& v)
	{
		LightGrid lg{{6, 6}};
		auto pos{lg.begin()};

		for (const std::vector<char>& subVec : v) {
			for (char c : subVec) {
				if (c == '#') {
					*pos = 1;
				} else {
					*pos = 0;
				}
				++pos;
			}
		}

		return lg;
	}

	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const LightGrid lg1{createLG({
				{'.', '#', '.', '#', '.', '#'},
				{'.', '.', '.', '#', '#', '.'},
				{'#', '.', '.', '.', '.', '#'},
				{'.', '.', '#', '.', '.', '.'},
				{'#', '.', '#', '.', '.', '#'},
				{'#', '#', '#', '#', '.', '.'},
			})};

			const LightGrid lg2{createLG({
				{'#', '#', '.', '#', '.', '#'},
				{'.', '.', '.', '#', '#', '.'},
				{'#', '.', '.', '.', '.', '#'},
				{'.', '.', '#', '.', '.', '.'},
				{'#', '.', '#', '.', '.', '#'},
				{'#', '#', '#', '#', '.', '#'},
			})};

			io.startTests();

			io.printTest(animateLights(lg1, 1), 11);
			io.printTest(animateLights(lg1, 2), 8);
			io.printTest(animateLights(lg1, 3), 4);
			io.printTest(animateLights(lg1, 4), 4);

			io.printTest(animateLights(lg2, 1, true), 18);
			io.printTest(animateLights(lg2, 2, true), 18);
			io.printTest(animateLights(lg2, 3, true), 18);
			io.printTest(animateLights(lg2, 4, true), 14);
			io.printTest(animateLights(lg2, 5, true), 17);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D18 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d18}};
		testPuzzle(io);
		
		constexpr int animationCount{100};
		LightGrid startGrid{readLightGrid(io.getInputFile())};

		io.printSolution(animateLights(startGrid, animationCount), 1061);
		io.printSolution(animateLights(startGrid, animationCount, true), 1006);
	}
}