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
		const std::size_t replacementSize{in.size() - 2}; // without empty line and medicineMolecule line
		CalibrationData result{
			{},
			*std::prev(in.cend()) // medicineMolecule is at the last line
		};
		result.replacements.reserve(replacementSize);

		std::for_each(in.cbegin(), std::next(in.cbegin(), replacementSize), [&](const std::string& str) {
			result.replacements.push_back(readReplacement(std::stringstream{str}));
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
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const CalibrationData test1{
				{{"H", "HO"}, {"H", "OH"}, {"O", "HH"}},
				"HOH",
			};
			const CalibrationData test2{
				{{"H", "HO"}, {"H", "OH"}, {"O", "HH"}},
				"HOHOHO",
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