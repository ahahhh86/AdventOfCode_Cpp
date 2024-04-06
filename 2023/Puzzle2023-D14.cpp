module;

#include <algorithm>
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
	using Platform = std::vector<std::vector<Rock>>;



	auto charToRock(char c)
	{
		switch (c) {
		case 'O': return Rock::rounded;
		case '#': return Rock::cubeShaped;
		case '.': return Rock::empty;
		default: throw AOC::InvalidFileInput{};
		}
	}



	auto readPlatform(std::vector<std::string>&& input)
	{
		Platform platform{};
		platform.reserve(input.size());

		std::ranges::for_each(input, [&](std::string_view line) {
			if (line.size() != input[0].size()) { throw AOC::InvalidInputData{"readPlatform(): line sizes do not match"}; }

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



	void moveRock(Platform& platform, Position pos, Position direction)
	{
		if (platform[pos.X()][pos.Y()] != Rock::rounded) return;

		Position newPos{pos};
		const Position lowerRight{static_cast<int>(platform.size()) - 1, static_cast<int>(platform[0].size()) - 1};
		do {
			newPos += direction;
		} while (newPos.isInBounds({0,0}, lowerRight) && platform[newPos.X()][newPos.Y()] == Rock::empty);
		newPos -= direction;

		if (newPos != pos) {
			std::swap(platform[newPos.X()][newPos.Y()], platform[pos.X()][pos.Y()]);
		}
	}



	void tiltPlatformNorth(Platform& platform)
	{
		const int sizeX{static_cast<int>(platform.size())};
		const int sizeY{static_cast<int>(platform[0].size())};

		for (int i{1}; i < sizeX; ++i) {
			for (int j{0}; j < sizeY; ++j) {
				moveRock(platform, {i, j}, north);
			}
		}
	}



	void tiltPlatformSouth(Platform& platform)
	{
		const int sizeX{static_cast<int>(platform.size())};
		const int sizeY{static_cast<int>(platform[0].size())};

		for (int i{sizeX - 2}; i >= 0; --i) {
			for (int j{0}; j < sizeY; ++j) {
				moveRock(platform, {i, j}, south);
			}
		}
	}



	void tiltPlatformWest(Platform& platform)
	{
		const int sizeX{static_cast<int>(platform.size())};
		const int sizeY{static_cast<int>(platform[0].size())};

		for (int i{0}; i < sizeX; ++i) {
			for (int j{1}; j < sizeY; ++j) {
				moveRock(platform, {i, j}, west);
			}
		}
	}



	void tiltPlatformEast(Platform& platform)
	{
		const int sizeX{static_cast<int>(platform.size())};
		const int sizeY{static_cast<int>(platform[0].size())};

		for (int i{0}; i < sizeX; ++i) {
			for (int j{sizeY - 2}; j >= 0; --j) {
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

			//rotatePlatform(platform2);
			//io.printTest(calculateWeight(platform2), 64); // TODO: Fail

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