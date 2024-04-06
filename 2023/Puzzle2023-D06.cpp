module;

#include <algorithm>
#include <fstream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

module Puzzle2023:D06;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using Number = long long;
	using NumberVec = std::vector<Number>;



	struct Race
	{
		Number time{};
		Number distance{};
	};



	auto readVector(std::stringstream& in)
	{
		NumberVec result{};

		while (!in.eof()) {
			Number buffer{};
			in >> buffer;
			if (buffer < 1) { throw AOC::InvalidFileLine{0}; }
			result.push_back(buffer);
		}

		return result;
	}



	auto readTime(std::stringstream&& in)
	{
		std::string buffer{};
		in >> buffer;
		if (buffer != "Time:") { throw AOC::InvalidFileLine{0}; }

		return readVector(in);
	}



	auto readDistance(std::stringstream&& in)
	{
		std::string buffer{};
		in >> buffer;
		if (buffer != "Distance:") { throw AOC::InvalidFileLine{1}; }

		return readVector(in);
	}



	using Races = std::vector<Race>;
	Races readRaces(std::ifstream&& in)
	{
		std::string buffer{};
		std::getline(in, buffer);
		const auto times{readTime(std::stringstream{buffer})};

		std::getline(in, buffer);
		const auto distances{readDistance(std::stringstream{buffer})};

		if (times.size() != distances.size()) { throw AOC::InvalidFileLine{2}; }

		Races result{};
		for (std::size_t i{0}; i < times.size(); ++i) {
			result.push_back({times[i], distances[i]});
		}

		return result;
	}
}



namespace { // Calculations
	auto calculateDistance(Number holdTime, Number raceTime)
	{
		return (raceTime - holdTime) * holdTime;
	}



	Number countWaysToWin(const Race& race)
	{
		for (Number holdTime{1}; holdTime < race.time; ++holdTime) {
			if (race.distance < calculateDistance(holdTime, race.time)) {
				// calculateDistance() is symmetric with a peak in the middle,
				// so we can abort, if we find the first way to win
				return race.time - holdTime * 2 + 1;
			}
		}

		return 0;
	}



	auto accumulateMarginOfError(const Races& races)
	{
		return std::accumulate(races.cbegin(), races.cend(), 1LL, [&](Number prod, const Race& race) {
			return prod * countWaysToWin(race);
		});
	}



	Race shrinkRaces(const Races& races)
	{
		std::stringstream time{};
		std::stringstream distance{};

		std::ranges::for_each(races, [&](const Race& race) {
			time << race.time;
			distance << race.distance;
		});

		return {std::stoll(time.str()), std::stoll(distance.str())};
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const Races testRaces{{7, 9}, {15, 40}, {30, 200}};

			io.startTests();

			io.printTest(calculateDistance(1, 7), 6LL);
			io.printTest(calculateDistance(2, 7), 10LL);
			io.printTest(calculateDistance(3, 7), 12LL);
			io.printTest(calculateDistance(4, 7), 12LL);

			io.printTest(countWaysToWin({7, 9}), 4LL);
			io.printTest(countWaysToWin({15, 40}), 8LL);
			io.printTest(countWaysToWin({30, 200}), 9LL);

			io.printTest(accumulateMarginOfError(testRaces), 288LL);
			io.printTest(countWaysToWin(shrinkRaces(testRaces)), 71503LL);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D06 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d06}};
		testPuzzle(io);

		const auto races{readRaces(io.getInputFile())};
		io.printSolution(accumulateMarginOfError(races), 800280LL);
		io.printSolution(countWaysToWin(shrinkRaces(races)), 45128024LL);
	}
}