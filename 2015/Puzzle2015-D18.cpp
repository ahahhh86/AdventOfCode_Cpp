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

namespace {
	constexpr int grid_size{100};
	using lightGrid_t = AOC::Grid2D<int>;



	class LightGrid
	{
	public:
		LightGrid() = default;
		LightGrid(const LightGrid&) = default;
		LightGrid(LightGrid&&) = default;
		explicit LightGrid(std::ifstream input);
		~LightGrid() = default;



		LightGrid& operator=(const LightGrid&) = delete;
		LightGrid& operator=(LightGrid&&) = delete;



		auto accumulateData() const;
		void animateLights();
		int sumAdjacentLights(int x, int y);
		void turnOnEdges();


	private:
		lightGrid_t m_input{{grid_size, grid_size}};
		lightGrid_t m_new_lights{{grid_size, grid_size}}; // to avoid multiple allocations
	};



	LightGrid::LightGrid(std::ifstream input)
	{
		char buffer{};
		auto i{m_input.begin()};

		input >> buffer;
		while (!input.eof()) {
			switch (buffer) {
			case '.':
				*i = 0;
				std::advance(i, 1);
				break;

			case '#':
				*i = 1;
				std::advance(i, 1);
				break;

			case '\r':
			case '\n':
				// Do nothing
				break;

			default:
				throw AOC::InvalidFileInput();
				break;
			}
			input >> buffer;
		}
		m_new_lights = m_input;
	}



	auto LightGrid::accumulateData() const
	{
		return std::accumulate(m_input.cbegin(), m_input.cend(), 0);
	}



	void LightGrid::animateLights()
	{
		int numAdjacentLights{};

		for (int j, i = 0; i < grid_size; ++i) {
			for (j = 0; j < grid_size; ++j) {
				numAdjacentLights = sumAdjacentLights(i, j);
				int& currentLight = m_new_lights[{i, j}]; // just for readability

				if (m_input[{i, j}] != 0) {
					if ((numAdjacentLights == 2) || (numAdjacentLights == 3)) { // stay alive // @suppress("Avoid magic numbers")
						currentLight = 1;
					} else { // die
						currentLight = 0;
					}
				} else {
					if (numAdjacentLights == 3) { // be born // @suppress("Avoid magic numbers")
						currentLight = 1;
					} else { // stay dead
						currentLight = 0;
					}
				}
			}
		}

		m_input = m_new_lights;
	}



	int LightGrid::sumAdjacentLights(int x, int y)
	{
		int result = m_input[{x, y}] ? -1 : 0; // if middle is alive, reduce by one, else it must have been excluded from loop

		for (int j, i = -1; i < 2; ++i) {
			for (j = -1; j < 2; ++j) {
				int x_pos{x + i};
				int y_pos{y + j};

				if (x_pos < 0 || grid_size <= x_pos) continue;
				if (y_pos < 0 || grid_size <= y_pos) continue;

				if (m_input[{x_pos, y_pos}] != 0) {
					++result;
				}
			}
		}

		return result;
	}



	void LightGrid::turnOnEdges()
	{
		m_input[{0, 0}] = 1;
		m_input[{0, grid_size - 1}] = 1;
		m_input[{grid_size - 1, 0}] = 1;
		m_input[{grid_size - 1, grid_size - 1}] = 1;
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



namespace AOC::Y2015::D18 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d18}};
		//testPuzzle(io);
		constexpr int loop_end{100};
		LightGrid inputData{io.getInputFile()};
		LightGrid gridPart1{inputData};
		LightGrid gridPart2{inputData};

		for (int i{0}; i < loop_end; ++i) {
			gridPart1.animateLights();
		}
		io.printSolution(gridPart1.accumulateData(), 1061);

		gridPart2.turnOnEdges();
		for (int i{0}; i < loop_end; ++i) {
			gridPart2.animateLights();
			gridPart2.turnOnEdges();
		}
		io.printSolution(gridPart2.accumulateData(), 1006);

		//InputData inputData{io.getInputFile()};

		//inputData.permutate(eggnog_volume);
		//io.printSolution(inputData.getPermutations(), 1638);
		//inputData.permutatePart2(eggnog_volume);
		//io.printSolution(inputData.getPermutationsPart2(), 17);
	}
}