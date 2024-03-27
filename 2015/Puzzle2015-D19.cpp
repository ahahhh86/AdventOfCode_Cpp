module;

#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

module Puzzle2015:D19;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	struct Replacement
	{
		std::string find{""};
		std::string replace{""};
	};
	using ReplacementVector = std::vector<Replacement>;



	struct CalibrationData
	{
		ReplacementVector replacements{};

		// "electronReplacements" not needed for part 1
		// can only be used once in part 2, because we start form one "e"
		// and no replacements results in "e"
		ReplacementVector electronReplacements{};

		std::string medicineMolecule{""};
	};



	auto readReplacement(std::stringstream&& str)
	{
		Replacement result{};

		str >> result.find;
		std::string buffer{};
		str >> buffer;
		if (buffer != "=>") throw AOC::InvalidFileInput();
		str >> result.replace;

		return result;
	}



	auto readCalibrationData(std::vector<std::string>&& in)
	{
		const auto replacementSize{in.size() - 2}; // without empty line and medicineMolecule line
		CalibrationData result{
			{},
			{},
			*std::prev(in.cend()) // medicineMolecule is at the last line
		};
		result.replacements.reserve(replacementSize);

		std::for_each(in.cbegin(), std::next(in.cbegin(), static_cast<long long>(replacementSize)), [&](const std::string& str) {
			const Replacement buffer{readReplacement(std::stringstream{str})};
			if (buffer.find != "e") {
				result.replacements.push_back(buffer);
			} else {
				result.electronReplacements.push_back(buffer);
			}
		});

		return result;
	}
}


	
namespace { // Calculations
	auto getReplacements(const CalibrationData& calib)
	{
		std::set<std::string> distinctMolecules{};

		std::ranges::for_each(calib.replacements, [&](const auto& data) {
			const std::size_t findLength{data.find.length()};

			for (auto i{calib.medicineMolecule.find(data.find, 0)}; i != std::string::npos; i = calib.medicineMolecule.find(data.find, i + findLength)) {
				std::string buffer{calib.medicineMolecule};
				buffer.replace(i, findLength, data.replace);
				distinctMolecules.emplace(buffer);
			}
		});

		return distinctMolecules.size();
	}



	//TODO: Part 2
	//int replaceAll(std::string& str, std::string_view from, std::string_view to)
	//{
	//	auto start{str.find(from)};
	//	int steps{0};

	//	while (start != std::string::npos) {
	//		str.replace(start, from.length(), to);
	//		start = str.find(from, start + to.length() - 1);
	//		++steps;
	//	}

	//	return steps;
	//}


	auto deconstuctMedicineMolecule(const CalibrationData& calib)
	{

	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const CalibrationData test1{
				{{"H", "HO"}, {"H", "OH"}, {"O", "HH"}},
				{},
				"HOH",
			};
			const CalibrationData test2{
				{{"H", "HO"}, {"H", "OH"}, {"O", "HH"}},
				{},
				"HOHOHO",
			};

			const CalibrationData test3{
				{{"H", "HO"}, {"H", "OH"}, {"O", "HH"}},
				{{"e", "H"}, {"e", "O"}},
				"HOH", // 3 steps
			};

			const CalibrationData test4{
				{{"H", "HO"}, {"H", "OH"}, {"O", "HH"}},
				{{"e", "H"}, {"e", "O"}},
				"HOHOHO", // 6 steps
			};

			io.startTests();

			io.printTest(getReplacements(test1), 4ULL);
			io.printTest(getReplacements(test2), 7ULL);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D19 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d19}};
		testPuzzle(io);

		CalibrationData cd{readCalibrationData(io.readInputFile<std::string>())};
		io.printSolution(getReplacements(cd), std::size_t{518});
	}
}