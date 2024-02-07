module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D13;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using GuestPair = std::pair<std::string, std::string>;
	struct PotentialHappiness
	{
		GuestPair guests{};
		int happiness{};
	};



	auto readHappiness(std::ifstream& in)
	{
		std::string str{};
		in >> str;

		int num{};
		in >> num;

		if (in.fail()) throw AOC::InvalidFileInput();

		if (str == "gain") {
			return num;
		} else if (str == "lose") {
			return -num;
		} else {
			throw AOC::InvalidFileInput();
		}
	}



	auto& operator>>(std::ifstream& in, PotentialHappiness& potential)
	{
		// format example: Alice would *gain/lose* 2 happiness units by sitting next to Bob.
		in >> potential.guests.first;
		in.ignore(std::ssize("would "));
		potential.happiness = readHappiness(in);
		in.ignore(std::ssize("happiness units by sitting next to "));
		in >> potential.guests.second;
		potential.guests.second.pop_back(); // remove "."

		return in;
	}
}



namespace { // Calculations
	using HappinessVector = std::vector<PotentialHappiness>;
	using GuestList = std::vector<std::string>;
	auto createGuestList(const HappinessVector& happyList)
	{
		GuestList result{};

		std::ranges::for_each(happyList, [&](const PotentialHappiness& p) {
			result.push_back(p.guests.first);
		});

		std::ranges::sort(result);
		const auto remove{std::ranges::unique(result)};
		result.erase(remove.begin(), remove.end());

		return result;
	}



	auto findHappiness(const GuestPair& guests, const HappinessVector& v)
	{
		const auto h{std::ranges::find_if(v, [&](const auto& p) {
			return p.guests == guests;
		})};
		if (h != v.cend()) {
			return h->happiness;
		} else {
			return 0;
		}
	}



	auto calculateHappiness(const GuestList& guests, const HappinessVector& v)
	{
		const auto loopEnd{std::prev(guests.cend())};
		auto result{findHappiness({*guests.cbegin(), *loopEnd}, v)
				+ findHappiness({*loopEnd, *guests.cbegin()}, v)};

		for (auto i{guests.cbegin()}; i != loopEnd; ++i) {
			const auto j{std::next(i)};
			result += findHappiness({*i, *j}, v)
				+ findHappiness({*j, *i}, v);
		}

		return result;
	}



	auto getMaxHappiness(const HappinessVector& v, bool includeMe = false)
	{
		auto guests{createGuestList(v)};
		if (includeMe) guests.push_back("me");
		auto maxHappiness{std::numeric_limits<int>::min()};

		do {
			maxHappiness = std::max(maxHappiness, calculateHappiness(guests, v));
		} while (std::next_permutation(guests.begin(), guests.end()));

		return maxHappiness;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			HappinessVector potential{
				{{"Alice", "Bob"}, 54},
				{{"Alice", "Carol"}, -79},
				{{"Alice", "David"}, -2},
				{{"Bob", "Alice"}, 83},
				{{"Bob", "Carol"}, -7},
				{{"Bob", "David"}, -63},
				{{"Carol", "Alice"}, -62},
				{{"Carol", "Bob"}, 60},
				{{"Carol", "David"}, 55},
				{{"David", "Alice"}, 46},
				{{"David", "Bob"}, -7},
				{{"David", "Carol"}, 41},
			};

			io.startTests();

			io.printTest(getMaxHappiness(potential), 330);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D13 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d13}};
		testPuzzle(io);

		const auto potential{io.readInputFile<PotentialHappiness>()};
		io.printSolution(getMaxHappiness(potential), 733);
		io.printSolution(getMaxHappiness(potential, true), 725);
	}
}