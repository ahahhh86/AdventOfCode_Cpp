module;

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

module Puzzle2015:D16;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using Compound = std::pair<std::string, int>;
	using AuntCompounds = std::vector<Compound>;

	struct AuntProperties
	{
		int number{};				// in case the aunt list is not sorted
		AuntCompounds compounds{};
	};

	using AuntVector = std::vector<AuntProperties>;



	auto readCompounds(std::stringstream&& in)
	{
		// format example: goldfish: 6, trees: 9, akitas: 0
		AuntCompounds result{};
		Compound buffer{};

		while (!in.eof()) {
			in >> buffer.first;
			buffer.first.pop_back(); // remove ':'
			in >> buffer.second;

			if (in.fail()) { throw AOC::InvalidFileInput{}; }
			result.push_back(buffer);

			in.ignore(std::ssize(","));
		}

		return result;
	}



	auto& operator>>(std::ifstream& in, AuntProperties& properties)
	{
		// format example: Sue 1: goldfish: 6, trees: 9, akitas: 0
		std::string str{};
		in >> str;		// Sue
		in >> properties.number;
		in.ignore(2);	// ": "

		std::getline(in, str);
		properties.compounds = readCompounds(std::stringstream{str});

		if (in.fail()) { throw AOC::InvalidFileInput{}; }

		return in;
	}
}



namespace { // Calculations
	bool matchesTape(const Compound& c, bool Part2 = false)
	{
		static std::map<std::string, int> tickerTape {
			std::make_pair("children", 3),
			std::make_pair("cats", 7),
			std::make_pair("samoyeds", 2),
			std::make_pair("pomeranians", 3),
			std::make_pair("akitas", 0),
			std::make_pair("vizslas", 0),
			std::make_pair("goldfish", 5),
			std::make_pair("trees", 3),
			std::make_pair("cars", 2),
			std::make_pair("perfumes", 1),
		};

		if (!tickerTape.contains(c.first)) { throw AOC::InvalidInputData{"matchesTape(): Compound not found."}; }

		if (Part2) {
			if (c.first == "cats" || c.first == "trees") {
				return tickerTape[c.first] < c.second;
			} else if (c.first == "pomeranians" || c.first == "goldfish") {
				return tickerTape[c.first] > c.second;
			}
		}

		return tickerTape[c.first] == c.second;
	}



	bool isAunt(const AuntCompounds& ac, bool Part2 = false)
	{
		for (const Compound& c : ac) {
			if (!matchesTape(c, Part2)) { return false; }
		};

		return true;
	}



	auto findAunt(const AuntVector& av, bool Part2 = false)
	{
		for (const AuntProperties& p : av) {
			if (isAunt(p.compounds, Part2)) { return p.number; }
		}

		throw AOC::aocError{"findAunt(): Aunt not found!"};
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			AuntCompounds testTape1{
				std::make_pair("children", 3),
				std::make_pair("cats", 7),
				std::make_pair("samoyeds", 2),
				std::make_pair("pomeranians", 3),
				std::make_pair("akitas", 0),
				std::make_pair("vizslas", 0),
				std::make_pair("goldfish", 5),
				std::make_pair("trees", 3),
				std::make_pair("cars", 2),
				std::make_pair("perfumes", 1),
			};

			AuntCompounds testTape2{
				std::make_pair("children", 3),
				std::make_pair("cats", 7),
				std::make_pair("samoyeds", 2),
				std::make_pair("pomeranians", 3),
				std::make_pair("akitas", 0),
				std::make_pair("vizslas", 1),
				std::make_pair("goldfish", 5),
				std::make_pair("trees", 3),
				std::make_pair("cars", 2),
				std::make_pair("perfumes", 1),
			};

			io.startTests();

			io.printTest(isAunt(testTape1), true);
			io.printTest(isAunt(testTape2), false);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D16 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d16}};
		testPuzzle(io);

		const auto auntVector{io.readInputFile<AuntProperties>()};

		io.printSolution(findAunt(auntVector), 103);
		io.printSolution(findAunt(auntVector, true), 405);
	}
}