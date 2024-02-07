module;

#include <algorithm>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

module Puzzle2015:D06;

import BasicImports;
import Grid2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Toggle
	{
		turnOn,
		turnOff,
		toggle,
	};



	auto& operator>>(std::ifstream& in, Toggle& toggle)
	{
		std::string buffer{};

		in >> buffer;
		if (buffer == "turn") {
			in >> buffer;

			if (buffer == "on") {
				toggle = Toggle::turnOn;
			} else if (buffer == "off") {
				toggle = Toggle::turnOff;
			}

		} else if (buffer == "toggle") {
			toggle = Toggle::toggle;

		} else {
			throw AOC::InvalidFileInput();
		}

		return in;
	}



	constexpr int gridSize{1000};
	using Point = AOC::Point2D<int>;
	constexpr Point invalidPos{-1, -1};

	struct Instruction
	{
		Point from{invalidPos};
		Point to{invalidPos};
		Toggle toggle{};

		bool isValid() const;
	};



	bool Instruction::isValid() const
	{
		return from.x >= 0
			&& from.y >= 0
			&& to.x >= 0
			&& to.y >= 0

			&& from.x < gridSize
			&& from.y < gridSize
			&& to.y < gridSize
			&& to.y < gridSize;
	}



	auto& operator>>(std::ifstream& in, Instruction& instruction)
	{
		// format: turn on 0,0 through 0,0
		in >> instruction.toggle;
		in >> instruction.from;
		in.ignore(std::ssize(" trough "));
		in >> instruction.to;

		if (!instruction.isValid()) throw AOC::InvalidFileInput();

		return in;
	}
}



namespace { // Calculations
	using Light = int;

	namespace Part1 {
		static void setLight(Light& light, Toggle toggle)
		{
			switch (toggle) {
			case Toggle::turnOn:
				light = true;
				break;
			case Toggle::turnOff:
				light = false;
				break;
			case Toggle::toggle:
				light = light ? false : true; // "!" creates a warning
				break;
			}
		}
	}



	namespace Part2 {
		static void setLight(Light& light, Toggle toggle)
		{
			switch (toggle) {
			case Toggle::turnOn:
				++light;
				break;
			case Toggle::turnOff:
				light = std::max(light - 1, 0);
				break;
			case Toggle::toggle:
				light += 2;
				break;
			}
		}
	}



	using SetLightFunction = void(*)(Light&, Toggle);
	void setLights(const std::vector<Instruction>& instructions, AOC::Grid2D<int>& grid, SetLightFunction fcn)
	{
		std::ranges::for_each(instructions, [&](const auto& instrctn) {
			grid.forSub(instrctn.from, instrctn.to, [&](int& light) {
				fcn(light, instrctn.toggle);
			});
		});
	}



	int deployLights(const std::vector<Instruction>& instructions, SetLightFunction fcn)
	{
		AOC::Grid2D<int> grid{{gridSize, gridSize}};
		setLights(instructions, grid, fcn);
		return std::accumulate(grid.cbegin(), grid.cend(), 0);
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			io.printTest(deployLights({{{0, 0},{999, 999},Toggle::turnOn}}, Part1::setLight), 1'000'000);
			io.printTest(deployLights({{{0, 0},{999, 0},Toggle::toggle}}, Part1::setLight), 1000);
			io.printTest(deployLights({{{499, 499},{500, 500},Toggle::turnOff}}, Part1::setLight), 0); // TODO: make better test

			io.printTest(deployLights({{{0, 0},{0, 0},Toggle::turnOn}}, Part2::setLight), 1);
			io.printTest(deployLights({{{0, 0},{999, 999},Toggle::toggle}}, Part2::setLight), 2'000'000);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D06 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d06}};
		testPuzzle(io);

		const auto input{io.readInputFile<Instruction>()};
		io.printSolution(deployLights(input, Part1::setLight), 377891);
		io.printSolution(deployLights(input, Part2::setLight), 14110788);
	}
}