module;

#include <fstream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

module Puzzle2023:D09;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using History = std::vector<int>;
	using Report = std::vector<History>;



	auto readHistory(std::stringstream&& in)
	{
		History result{};

		while (!in.eof()) {
			int buffer{};
			in >> buffer;
			result.push_back(buffer);
		}

		return result;
	}



	auto& operator>>(std::ifstream& in, History& game)
	{
		std::string readLine{};
		std::getline(in, readLine);
		game = readHistory(std::stringstream{readLine});

		return in;
	}
}



namespace { // Calculations
	bool areNumbersSame(const History& h)
	{
		for (int i : h) {
			if (i != h[0]) return false;
		}

		return true;
	}


	// first is the first part of the puzzle, but the number *after* the history
	// second is the second part of the puzzle, but the number *before* the history
	using Extrapolated = std::pair<int, int>;
	Extrapolated operator+(const Extrapolated& p1, const Extrapolated& p2)
	{
		return {p1.first + p2.first, p1.second + p2.second};
	}



	Extrapolated extrapolateNumbers(const History& h)
	{
		if (h.size() == 0) throw AOC::InvalidInputData("extrapolateNumbers");
		if (areNumbersSame(h)) return {h[0], h[0]};

		const auto hSize{h.size()};
		History newH(hSize - 1);
		for (std::size_t i{1}; i < hSize; ++i) {
			newH[i - 1] = h[i] - h[i - 1];
		}

		Extrapolated result{extrapolateNumbers(newH)};
		result.first += h[hSize - 1];
		result.second = h[0] - result.second;
		return result;
	}



	auto accumulateNextNumbers(const Report& report)
	{
		return std::accumulate(report.cbegin(), report.cend(), Extrapolated{0, 0}, [&](auto sum, const History& h){
			return sum + extrapolateNumbers(h);
		});
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const Report test{
				{ 0,  3,  6,  9, 12, 15},
				{ 1,  3,  6, 10, 15, 21},
				{10, 13, 16, 21, 30, 45},
			};

			io.startTests();

			io.printTest(extrapolateNumbers(test[0]).first, 18);
			io.printTest(extrapolateNumbers(test[1]).first, 28);
			io.printTest(extrapolateNumbers(test[2]).first, 68);
			io.printTest(accumulateNextNumbers(test).first, 114);

			io.printTest(extrapolateNumbers(test[0]).second, -3);
			io.printTest(extrapolateNumbers(test[1]).second, 0);
			io.printTest(extrapolateNumbers(test[2]).second, 5);
			io.printTest(accumulateNextNumbers(test).second, 2);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D09 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d09}};
		testPuzzle(io);

		const auto numbers{accumulateNextNumbers(io.readInputFile<History>())};
		io.printSolution(numbers.first, 1993300041);
		io.printSolution(numbers.second, 1038);
	}
}