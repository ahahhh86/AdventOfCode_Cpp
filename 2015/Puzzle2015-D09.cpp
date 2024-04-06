module;

#include <algorithm>
#include <fstream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D09;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using Location = std::string;
	using LocationPair = std::pair<Location, Location>;

	constexpr int invalidDistance{0};	// Does not make sense, if two places are the same



	struct LocationDistance
	{
		LocationPair locations{"", ""};
		int distance{invalidDistance};

		bool isValid() const;
	};



	bool LocationDistance::isValid() const
	{
		return locations.first != ""
			&& locations.second != ""
			&& distance > invalidDistance;
	}



	auto& operator>>(std::ifstream& in, LocationDistance& data)
	{
		// format example: Faerun to Tristram = 65
		in >> data.locations.first;
		in.ignore(std::ssize("to "));
		in >> data.locations.second;
		in.ignore(std::ssize("= "));
		in >> data.distance;

		if (!data.isValid()) { throw AOC::InvalidFileInput{} ;}

		return in;
	}
}



namespace { // Calculations
	using Distances = std::vector<LocationDistance>;
	using LocationVec = std::vector<std::string_view>;	// not std::set, because needs to be permutable

	auto createLocationVec(const Distances& source)
	{
		std::set<std::string_view> set{};
		std::ranges::for_each(source, [&](const LocationDistance& loc){
			set.insert(loc.locations.first);
			set.insert(loc.locations.second);
		});

		LocationVec vec{set.begin(), set.end()};
		std::ranges::sort(vec);
		return vec;
	}



	using LocationMap = std::map<LocationPair, int>;
	auto createLocationMap(const Distances& source)
	{
		LocationMap result{};

		std::ranges::for_each(source, [&](const LocationDistance& loc) {
			result.insert({loc.locations, loc.distance});
		});

		return result;
	}



	auto getLocationDistance(LocationPair loc, const LocationMap& map)
	{

		if (map.contains(loc)) {
			return map.at(loc);
		}

		std::swap(loc.first, loc.second);
		if (map.contains(loc)) {
			return map.at(loc);
		}

		throw AOC::InvalidInputData{"getLocationDistance()"};
	}



	auto calculateTravelDistance(const LocationVec& locations, const LocationMap& map)
	{
		int result{0};

		for (auto loc{std::next(locations.cbegin())}; loc != locations.cend(); ++loc) {
			const auto prevLoc{std::prev(loc)};
			result += getLocationDistance(LocationPair{*prevLoc, *loc}, map);
		}

		return result;
	}



	struct PuzzleResult
	{
		int minTravelDistance{std::numeric_limits<int>::max()};
		int maxTravelDistance{std::numeric_limits<int>::min()};
	};



	auto calculateTravelDistances(const Distances& routes)
	{
		PuzzleResult result{};
		LocationVec locations{createLocationVec(routes)};
		const LocationMap map{createLocationMap(routes)};

		do {
			const int traveledDistance{calculateTravelDistance(locations, map)};

			result.minTravelDistance = std::min(result.minTravelDistance, traveledDistance);
			result.maxTravelDistance = std::max(result.maxTravelDistance, traveledDistance);
		} while (std::next_permutation(locations.begin(), locations.end()));

		return result;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			Distances input{
				{{"London", "Dublin"}, 464},
				{{"London", "Belfast"}, 518},
				{{"Dublin", "Belfast"}, 141},
			};

			io.startTests();

			const auto result{calculateTravelDistances(input)};

			io.printTest(result.minTravelDistance, 605);
			io.printTest(result.maxTravelDistance, 982);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D09 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d09}};
		testPuzzle(io);

		const auto result{calculateTravelDistances(io.readInputFile<LocationDistance>())};
		io.printSolution(result.minTravelDistance, 117);
		io.printSolution(result.maxTravelDistance, 909);
	}
}