module;

#include <algorithm>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D11;

import BasicImports;
import Point2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *
namespace { // Input
	using GalaxyMap = std::vector<std::string>;
	using Number = long long;
	using NumVec = std::vector<Number>;
	using Galaxy = AOC::Point2D<Number>;
	using Galaxies = std::vector<Galaxy>;

	constexpr char charGalaxy{'#'};
	constexpr char charEmpty{'.'};



	void validate(const GalaxyMap& map)
	{
		const auto differentLineLength{std::ranges::count_if(map, [&](std::string_view line) {
			return line.size() != map[0].size();
		})};

		if (differentLineLength != 0) {throw AOC::InvalidInputData("validate()");}
	}



	auto findEmptyLine(const GalaxyMap& map)
	{
		NumVec result{};

		for (std::size_t line{0}; line < map.size(); ++line) {
			bool foundGalaxy{false};

			for (std::size_t pos{0}; pos < map[line].size(); ++pos) {
				if (map[line][pos] == charGalaxy) {
					foundGalaxy = true;
					break;
				}
			}

			if (!foundGalaxy) {
				result.push_back(static_cast<Number>(line));
			}
		}

		return result;
	}



	auto findEmptyCol(const GalaxyMap& map)
	{
		NumVec result{};

		for (std::size_t pos{0}; pos < map[0].size(); ++pos) {
			bool foundGalaxy{false};

			for (std::size_t line{0}; line < map.size(); ++line) {
				if (map[line][pos] == charGalaxy) {
					foundGalaxy = true;
					break;
				}
			}

			if (!foundGalaxy) {
				result.push_back(static_cast<Number>(pos));
			}
		}

		return result;
	}



	auto findGalaxies(const GalaxyMap& map)
	{
		Galaxies result{};

		for (std::size_t line{0}; line < map.size(); ++line) {
			for (std::size_t pos{0}; pos < map[line].size(); ++pos) {
				if (map[line][pos] == charGalaxy) {
					result.push_back({static_cast<Number>(line), static_cast<Number>(pos)});
				} else if (map[line][pos] != charEmpty) {
					throw AOC::InvalidInputData("findGalaxies()");
				}
			}
		}

		return result;
	}
}



namespace { // Calculations
	auto getExpanded(const GalaxyMap& map)
	{
		const auto emptyLine{findEmptyLine(map)};
		const auto emptyCol{findEmptyCol(map)};
		Galaxies result{findGalaxies(map)};

		std::ranges::for_each(result, [&](Galaxy& galaxy) {
			const auto emptyLineBefore{std::ranges::count_if(emptyLine, [&](Number i){
				return i < galaxy.x;
			})};
			const auto emptyColBefore{std::ranges::count_if(emptyCol, [&](Number i) {
				return i < galaxy.y;
			})};

			galaxy += Galaxy{static_cast<Number>(emptyLineBefore), static_cast<Number>(emptyColBefore)};
		});

		return result;
	}



	auto getExpanded(const GalaxyMap& map, Number factor)
	{
		const auto emptyLine{findEmptyLine(map)};
		const auto emptyCol{findEmptyCol(map)};
		Galaxies result{findGalaxies(map)};

		std::ranges::for_each(result, [&](Galaxy& galaxy) {
			const auto emptyLineBefore{std::ranges::count_if(emptyLine, [&](Number i) {
				return i < galaxy.x;
			})};
			const auto emptyColBefore{std::ranges::count_if(emptyCol, [&](Number i) {
				return i < galaxy.y;
			})};

			galaxy += Galaxy{static_cast<Number>(emptyLineBefore) * (factor - 1), static_cast<Number>(emptyColBefore) * (factor - 1)};
		});

		return result;
	}



	auto calculateDistance(Number start, Number end)
	{
		return std::llabs(end - start);
	}



	auto calculateDistance(Galaxy g1, Galaxy g2)
	{
		const auto lineDistance{calculateDistance(g1.x, g2.x)};
		const auto colDistance{calculateDistance(g1.y, g2.y)};

		return lineDistance + colDistance;
	}



	auto accumulateDistance(const Galaxies& galaxies)
	{
		Number result{0};
		for (auto i{galaxies.cbegin()}; i != galaxies.cend(); ++i) {
			for (auto j{std::next(i)}; j != galaxies.cend(); ++j) {
				const Number dist{calculateDistance(*i, *j)};
				result += dist;
			}
		}
		return result;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const GalaxyMap test{
				"...#......",
				".......#..",
				"#.........",
				"..........",
				"......#...",
				".#........",
				".........#",
				"..........",
				".......#..",
				"#...#.....",
			};

			io.startTests();
			validate(test);
			const auto galaxies{getExpanded(test)};

			io.printTest(calculateDistance(galaxies[4], galaxies[8]), 9LL);
			io.printTest(calculateDistance(galaxies[0], galaxies[6]), 15LL);
			io.printTest(calculateDistance(galaxies[2], galaxies[5]), 17LL);
			io.printTest(calculateDistance(galaxies[7], galaxies[8]), 5LL);

			io.printTest(accumulateDistance(galaxies), 374LL);

			io.printTest(accumulateDistance(getExpanded(test, 10)), 1030LL);
			io.printTest(accumulateDistance(getExpanded(test, 100)), 8410LL);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D11 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d11}};
		testPuzzle(io);

		const auto map{io.readInputFile<std::string>()};
		validate(map);

		io.printSolution(accumulateDistance(getExpanded(map)), 9608724LL);
		io.printSolution(accumulateDistance(getExpanded(map, 1000000)), 904633799472LL);
	}
}