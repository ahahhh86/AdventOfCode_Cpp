module;

#include <algorithm>
#include <array>
#include <fstream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

module Puzzle2023:D05;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using Id = long long;
	using IdVector = std::vector<Id>;

	auto& operator>>(std::ifstream& in, IdVector& seeds)
	{
		while (!in.fail()) {
			Id buffer{};
			in >> buffer;
			seeds.push_back(buffer);
		}

		seeds.resize(seeds.size() - 1);						// last one will be 0
		in.clear(std::ios_base::failbit & std::ios::badbit);

		return in;
	}



	struct AlmanacMapLine
	{
		Id destinationStart{};
		Id sourceStart{};
		Id length{};
	};
	using AlmanacMap = std::vector<AlmanacMapLine>;

	auto& operator>>(std::ifstream& in, AlmanacMap& map)
	{
		while (!in.fail()) {
			AlmanacMapLine buffer{};
			in >> buffer.destinationStart;
			in >> buffer.sourceStart;
			in >> buffer.length;

			map.push_back(buffer);
		}

		map.resize(map.size() - 1);
		in.clear(std::ios_base::failbit & std::ios::badbit);

		return in;
	}



	auto& operator>>(std::ifstream& in, std::vector<AlmanacMap>& map)
	{
		using namespace std::string_view_literals;
		constexpr std::array headlines{
				"seed-to-soil map:"sv,
				"soil-to-fertilizer map:"sv,
				"fertilizer-to-water map:"sv,
				"water-to-light map:"sv,
				"light-to-temperature map:"sv,
				"temperature-to-humidity map:"sv,
				"humidity-to-location map:"sv,
		};

		std::size_t index{1};
		std::ranges::for_each(headlines, [&](std::string_view hl) {
			std::string bufferStr{};
			std::getline(in, bufferStr);
			if (bufferStr != hl) throw AOC::InvalidFileLine(index);
			++index;

			AlmanacMap bufferMap{};
			in >> bufferMap;
			map.push_back(bufferMap);
		});

		return in;
	}



	struct Almanac
	{
		IdVector seeds{};
		std::vector<AlmanacMap> maps{};
	};




	auto readInput(std::ifstream&& in)
	{
		Almanac result{};

		std::string buffer{};
		in >> buffer;
		if (buffer != "seeds:") throw AOC::InvalidFileInput();
		in >> result.seeds;
		in >> result.maps;

		return result;
	}
}



namespace { // Calculations
	Id calculateNewId(Id oldId, const AlmanacMap& map)
	{
		for (AlmanacMapLine line : map) {
			const auto sourceEnd{line.sourceStart + line.length};
			if (line.sourceStart <= oldId && oldId < sourceEnd) {
				return line.destinationStart + oldId - line.sourceStart;
			}
		}

		return oldId;
	}



	auto calculateLocation(Id seed, const Almanac& almanac)
	{
		const auto soil{calculateNewId(seed, almanac.maps[0])};
		const auto fertilizer{calculateNewId(soil, almanac.maps[1])};
		const auto water{calculateNewId(fertilizer, almanac.maps[2])};
		const auto light{calculateNewId(water, almanac.maps[3])};
		const auto temperature{calculateNewId(light, almanac.maps[4])};
		const auto humidity{calculateNewId(temperature, almanac.maps[5])};

		return calculateNewId(humidity, almanac.maps[6]);
	}



	namespace Part1 {
		static auto minLocation(const Almanac& almanac)
		{
			auto minLocation{std::numeric_limits<Id>::max()};

			std::ranges::for_each(almanac.seeds, [&](auto seed) {
				minLocation = std::min(calculateLocation(seed, almanac), minLocation);
			});

			return minLocation;
		}
	}



	namespace Part2 {
		static auto minLocation(const Almanac& almanac)
		{
			if (almanac.seeds.size() % 2 != 0) throw AOC::InvalidInputData("Part2::minLocation");

			auto minLocation{std::numeric_limits<Id>::max()};

			for (std::size_t i{1}; i < almanac.seeds.size(); i += 2) {
				const auto seedStart{almanac.seeds[i - 1]};
				const auto seedEnd{seedStart + almanac.seeds[i]};

				for (Id j{seedStart}; j < seedEnd; ++j) {
					minLocation = std::min(calculateLocation(j, almanac), minLocation);
				}
			}

			return minLocation;
		}
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			Almanac test{
				// seeds
				{79, 14, 55, 13},
				// seed-to-soil map:
				{{{50, 98, 2},
				{52, 50, 48}},
				// soil-to-fertilizer map:
				{{0, 15, 37},
				{37, 52, 2},
				{39, 0, 15}},
				// fertilizer-to-water map:
				{{49, 53, 8},
				{0, 11, 42},
				{42, 0, 7},
				{57, 7, 4}},
				// water-to-light map:
				{{88, 18, 7},
				{18, 25, 70}},
				// light-to-temperature map:
				{{45, 77, 23},
				{81, 45, 19},
				{68, 64, 13}},
				// temperature-to-humidity map:
				{{0, 69, 1},
				{1, 0, 69}},
				// humidity-to-location map:
				{{60, 56, 37},
				{56, 93, 4}}},
			};

			io.startTests();

			io.printTest(calculateNewId(98, test.maps[0]), 50LL);
			io.printTest(calculateNewId(99, test.maps[0]), 51LL);
			io.printTest(calculateNewId(53, test.maps[0]), 55LL);
			io.printTest(calculateNewId(10, test.maps[0]), 10LL);

			io.printTest(calculateLocation(79, test), 82LL);
			io.printTest(calculateLocation(14, test), 43LL);
			io.printTest(calculateLocation(55, test), 86LL);
			io.printTest(calculateLocation(13, test), 35LL);

			io.printTest(Part1::minLocation(test), 35LL);
			io.printTest(Part2::minLocation(test), 46LL);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D05 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d05}};
		testPuzzle(io);

		const auto almanac{readInput(io.getInputFile())};
		io.printSolution(Part1::minLocation(almanac), 331445006LL);
		io.printSolution(Part2::minLocation(almanac), 6472060LL);		// ca. 3:30 min with release-build on my computer
	}
}