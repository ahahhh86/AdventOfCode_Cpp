module;

#include <algorithm>
#include <fstream>
#include <numeric>
#include <optional>
#include <vector>

module Puzzle2015:D24;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Calculations
	using Packages = std::vector<int>;

	auto calculateGroupWeight(const Packages& packages)
	{
		constexpr int groupCount{3};
		const auto sumWeight{std::accumulate(packages.cbegin(), packages.cend(), 0)};
		if (sumWeight % groupCount != 0) { throw AOC::InvalidInputData{"calculateGroupWeight(): weight not dividable by groups"}; }
		return sumWeight / groupCount;
	}



	struct Compartments
	{
		Packages passenger{};
		Packages left{};
		Packages right{};

		auto getPassengerQE() const;
	};
	using CompartmentResult = std::optional<Compartments>;
	using CompartmentVector = std::vector<Compartments>;

	auto Compartments::getPassengerQE() const
	{
		return std::accumulate(passenger.cbegin(), passenger.cend(), 1, std::multiplies());
	}



	bool operator<(const Compartments& c1, const Compartments& c2)
	{
		if (c1.passenger.size() == c2.passenger.size()) {
			return c1.getPassengerQE() < c2.getPassengerQE();
		} else {
			return c1.passenger.size() < c2.passenger.size();
		}
	}



	auto getDivider(Packages::const_iterator begin, Packages::const_iterator end, int groupWeight)
	{
		for (auto i{begin}; i != end; ++i) {
			const auto weight{std::accumulate(begin, std::next(i), 0)};

			if (weight > groupWeight) { return end; }
			if (weight == groupWeight) {
				return std::next(i);
			}
		}

		return end;
	}



	CompartmentResult divideIntoGroups(const Packages& packages, int groupWeight)
	{
		const auto begin{packages.cbegin()};
		const auto end{packages.cend()};

		const auto mid1{getDivider(begin, end, groupWeight)};
		if (mid1 == end) { return {}; }

		const auto mid2{getDivider(mid1, end, groupWeight)};
		if (mid2 == end) { return {}; }

		Compartments result{
			{begin, mid1},
			{mid1, mid2},
			{mid2, end}
		};

		return result;
	}



	auto sortPackages(Packages packages)
	{
		const auto groupWeight{calculateGroupWeight(packages)};
		std::ranges::sort(packages);
		CompartmentVector packageDivision{};

		do {
			const auto division{divideIntoGroups(packages, groupWeight)};
			if (division) { packageDivision.push_back(*division); }
		} while (std::next_permutation(packages.begin(), packages.end()));

		std::ranges::sort(packageDivision, operator<);
		return packageDivision[0].getPassengerQE();
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const Packages test{1, 2, 3, 4, 5, 7, 8, 9, 10, 11};

			io.startTests();

			io.printTest(sortPackages(test), 99);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D24 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d24}};
		testPuzzle(io);

		const auto packages{io.readInputFile<int>()};
		//io.printSolution(sortPackages(packages), 99); // TODO: too slow
	}
}