module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D19;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	struct SReplacement
	{
		std::string find{""};
		std::string replace{""};
	};
	using replacements_t = std::vector<SReplacement>;

	struct SInputData
	{
		replacements_t replacements{};
		std::string medicineMolecule{""};
	};



	SInputData readInput(std::ifstream file)
	{
		SInputData result{};
		SReplacement buffer{};

		while (true) {
			constexpr auto sep_width{std::string_view("=> ").length()};
			constexpr std::size_t max_find_molecule_length{10}; // the start molecule in my input has 1 or 2 letters, while the medicine molecule has a lot more

			file >> buffer.find;
			if (buffer.find.length() > max_find_molecule_length) {
				break;
			}
			file.ignore(sep_width);
			file >> buffer.replace;
			result.replacements.push_back(buffer);
		}
		if(file.fail()) throw AOC::InvalidFileInput();

		result.medicineMolecule = buffer.find;
		return result;
	}


	
	auto getReplacements(const SInputData& input)
	{
		std::list<std::string> distinctMolecules{};
		std::string buffer{};

		std::for_each(input.replacements.cbegin(), input.replacements.cend(), [&](const auto& data) {
			const std::size_t findLength{data.find.length()};

			for (auto i{input.medicineMolecule.find(data.find, 0)}; i != std::string::npos; i = input.medicineMolecule.find(data.find, i + findLength)) {
				buffer = input.medicineMolecule;
				buffer.replace(i, findLength, data.replace);
				distinctMolecules.push_back(buffer);
			}
		});

		distinctMolecules.sort();
		distinctMolecules.unique();
		return distinctMolecules.size();
	}



	auto buildMedicineMolecule(const SInputData&/* input*/)
	{
		// TODO
		return std::size_t{0};//result;
	}
}



namespace { // Testing
	// TODO:
	//void testPuzzle(AOC::IO& io)
	//{
	//	if (AOC::debugMode) {
	//		const StatVector stats{
	//			{14, 10, 127},
	//			{16, 11, 162},
	//		};

	//		io.startTests();

	//		const auto raceResult{getRaceResults(stats, 1000)};
	//		io.printTest(raceResult.first, 1120);
	//		io.printTest(raceResult.second, 689);

	//		io.endTests();
	//	}
	//}
}



namespace AOC::Y2015::D19 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d19}};
		//testPuzzle(io);

		SInputData input{readInput(io.getInputFile())};

		io.printSolution(getReplacements(input), std::size_t{518});
		io.printSolution(buildMedicineMolecule(input), std::size_t{0});
	}
}