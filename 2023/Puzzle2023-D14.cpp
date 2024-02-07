module;

#include <algorithm>
#include <print>
#include <string>
#include <vector>

module Puzzle2023:D14;

import BasicImports;
import Point2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

// TODO: Part2 takes too long, need shortcut by using repetative pattern

namespace { // Input
	enum class Rock
	{
		rounded,
		cubeShaped,
		empty,
	};



	auto charToRock(char c)
	{
		switch (c) {
		case 'O': return Rock::rounded;
		case '#': return Rock::cubeShaped;
		case '.': return Rock::empty;
		default: throw AOC::InvalidFileInput();
		}
	}



	using Platform = std::vector<std::vector<Rock>>;
	auto readPlatform(std::vector<std::string>&& input)
	{
		Platform platform{};
		platform.reserve(input.size());

		std::ranges::for_each(input, [&](std::string_view line) {
			if (line.size() != input[0].size()) throw AOC::InvalidInputData("readPlatform");

			std::vector<Rock> buffer{};
			buffer.reserve(line.size());

			std::ranges::for_each(line, [&](char c) {
				buffer.push_back(charToRock(c));
			});

			platform.push_back(buffer);
		});

		return platform;
	}
}



namespace { // Calculations
	using Position = AOC::Point2D<int>;

	constexpr Position north{-1, 0};
	constexpr Position east{0, 1};
	constexpr Position south{1, 0};
	constexpr Position west{0, -1};

	bool inBounds(Platform& platform, Position pos)
	{
		return 0 <= pos.x && pos.x < platform.size() && 0 <= pos.y && pos.y < platform[0].size();
	}


	void moveRock(Platform& platform, Position pos, Position direction)
	{
		if (platform[pos.x][pos.y] != Rock::rounded) return;

		int distance{0};
		while (true) {
			++distance;
			if (!inBounds(platform, {pos.x + distance * direction.x, pos.y + distance * direction.y})) break;
			if (platform[pos.x + distance * direction.x][pos.y + distance * direction.y] != Rock::empty) break;
		}
		--distance;

		if (distance != 0) {
			std::swap(platform[pos.x + distance * direction.x][pos.y + distance * direction.y], platform[pos.x][pos.y]);
		}
	}



	void tiltPlatformNorth(Platform& platform)
	{
		for (int i{1}; i < platform.size(); ++i) {
			for (int j{0}; j < platform[i].size(); ++j) {
				moveRock(platform, {i, j}, north);
			}
		}
	}



	void tiltPlatformSouth(Platform& platform)
	{
		for (int i = platform.size() - 2; i >= 0; --i) {
			for (int j{0}; j < platform[i].size(); ++j) {
				moveRock(platform, {i, j}, south);
			}
		}
	}



	void tiltPlatformWest(Platform& platform)
	{
		for (int i{0}; i < platform.size(); ++i) {
			for (int j{1}; j < platform[i].size(); ++j) {
				moveRock(platform, {i, j}, west);
			}
		}
	}



	void tiltPlatformEast(Platform& platform)
	{
		for (int i{0}; i < platform.size(); ++i) {
			for (int j = platform[i].size() - 2; j >= 0; --j) {
				moveRock(platform, {i, j}, east);
			}
		}
	}



	auto calculateWeight(const Platform& platform)
	{
		auto weightFactor{static_cast<int>(platform.size())};
		int result{0};

		std::ranges::for_each(platform, [&](const std::vector<Rock>& line) {
			const auto count{std::ranges::count_if(line, [&](Rock rock) {
				return rock == Rock::rounded;
			})};

			result += static_cast<int>(count) * weightFactor;
			--weightFactor;
		});

		return result;
	}



	void rotatePlatform(Platform& platform)
	{
		// TODO: find repetative pattern and calculate result from it
		constexpr int count{1'00};//0'000'000
		for (int i{0}; i < count; ++i) {
			tiltPlatformNorth(platform);
			tiltPlatformWest(platform);
			tiltPlatformSouth(platform);
			tiltPlatformEast(platform);

			//std::println("{:03}: {:03}", i, calculateWeight(platform));
		}
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			auto platform{readPlatform({
				"O....#....",
				"O.OO#....#",
				".....##...",
				"OO.#O....O",
				".O.....O#.",
				"O.#..O.#.#",
				"..O..#O..O",
				".......O..",
				"#....###..",
				"#OO..#....",
			})};
			auto platform2{platform};

			io.startTests();

			tiltPlatformNorth(platform);
			io.printTest(calculateWeight(platform), 136);

			rotatePlatform(platform2);
			io.printTest(calculateWeight(platform2), 64);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D14 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d14}};
		testPuzzle(io);

		auto platform{readPlatform(io.readInputFile<std::string>())};
		auto platform2{platform};

		tiltPlatformNorth(platform);
		io.printSolution(calculateWeight(platform), 109755);
		//rotatePlatform(platform2);
		//io.printSolution(calculateWeight(platform2), 0);
	}
}