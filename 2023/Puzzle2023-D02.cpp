module;

#include <algorithm>
#include <fstream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

module Puzzle2023:D02;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	constexpr int invalidCubeNumber{-1};

	struct CubeCounts
	{
		int blue{0};
		int green{0};
		int red{0};
	};



	constexpr int invalidGame{0};
	struct Game
	{
		int number{invalidGame};			// could be removed, = index of GameVector - 1
		std::vector<CubeCounts> grab{};

		bool isValid() const;
	};



	bool Game::isValid() const
	{
		for (const CubeCounts& cubes : grab) {
			if (cubes.blue <= invalidCubeNumber || cubes.green <= invalidCubeNumber || cubes.red <= invalidCubeNumber) {
				return false;
			}
		}

		return number > invalidGame;
	}



	auto readCubes(std::stringstream& in)
	{
		// format example: "3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"
		CubeCounts result{};

		while (!in.eof()) {
			int cubeCount{};
			in >> cubeCount;

			if (in.fail()) throw AOC::InvalidFileInput();

			std::string cubeColour{};
			in >> cubeColour;
			if (cubeColour.starts_with("blue")) {
				result.blue = cubeCount;
			} else if (cubeColour.starts_with("green")) {
				result.green = cubeCount;
			} else if (cubeColour.starts_with("red")) {
				result.red = cubeCount;
			} else {
				throw AOC::InvalidFileInput();
			}

			if (cubeColour.ends_with(";")) break;
		}

		return result;
	}



	auto readGame(std::stringstream&& in)
	{
		// format example: "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"
		Game result{};
		std::string buffer{};

		in >> buffer;
		if (buffer != "Game") throw AOC::InvalidFileInput();

		in >> result.number;
		in >> buffer;
		if (buffer != ":") throw AOC::InvalidFileInput();

		while (!in.eof()) {
			result.grab.push_back(readCubes(in));
		}

		return result;
	}



	auto& operator>>(std::ifstream& in, Game& game)
	{
		std::string readLine{};
		std::getline(in, readLine);
		game = readGame(std::stringstream{readLine});
		if (!game.isValid()) throw AOC::InvalidFileInput();
		return in;
	}
}



namespace { // Calculations
	int getNumberIfPossible(const Game& game)
	{
		constexpr int maxBlueCount{14};
		constexpr int maxGreenCount{13};
		constexpr int maxRedCount{12};

		for (const CubeCounts& cubes : game.grab) {
			if (cubes.blue > maxBlueCount || cubes.green > maxGreenCount || cubes.red > maxRedCount) return invalidGame;
		};

		return game.number;
	}



	using GameVector = std::vector<Game>;
	auto accumulatePossibleGames(const GameVector& games)
	{
		return std::accumulate(games.cbegin(), games.cend(), 0, [&](int sum, const Game& game) {
			return sum + getNumberIfPossible(game);
		});
	}



	auto getMinCubeCount(const Game& game)
	{
		CubeCounts result{0, 0, 0};

		std::ranges::for_each(game.grab, [&](const CubeCounts& cubes) {
			result.blue = std::max(result.blue, cubes.blue);
			result.green = std::max(result.green, cubes.green);
			result.red = std::max(result.red, cubes.red);
		});

		return result;
	}



	auto getCubePower(const CubeCounts& cubes)
	{
		return cubes.blue * cubes.green * cubes.red;
	}



	auto accumulateCubePower(const GameVector& games)
	{
		return std::accumulate(games.cbegin(), games.cend(), 0, [&](int sum, const Game& game) {
			return sum + getCubePower(getMinCubeCount(game));
		});
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const GameVector testGames{
				readGame(std::stringstream{"Game 1 : 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"}),
				readGame(std::stringstream{"Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue"}),
				readGame(std::stringstream{"Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red"}),
				readGame(std::stringstream{"Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red"}),
				readGame(std::stringstream{"Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green"}),
			};

			io.startTests();

			io.printTest(accumulatePossibleGames(testGames), 8);
			io.printTest(accumulateCubePower(testGames), 2286);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D02 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d02}};
		testPuzzle(io);

		const auto games{io.readInputFile<Game>()};
		io.printSolution(accumulatePossibleGames(games), 2105);
		io.printSolution(accumulateCubePower(games), 72422);
	}
}