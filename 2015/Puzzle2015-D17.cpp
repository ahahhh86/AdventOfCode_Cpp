module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

module Puzzle2015:D17;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	using vecSize_t = std::vector<std::string>::size_type;
	constexpr int eggnog_volume{150};

	class InputData
	{
	public:
		InputData() = delete;
		InputData(const InputData&) = delete;
		InputData(InputData&&) = delete;
		explicit InputData(std::ifstream&& input);
		~InputData() = default;

		InputData& operator=(const InputData&) = delete;
		InputData& operator=(InputData&&) = delete;


		int getPermutations() const
			{return m_permutations;}
		int getPermutationsPart2() const
			{return m_permutationsPart2;}

		void permutate(int freeVolume, const vecSize_t startPos = 0, int usedContainers = 0);
		void permutatePart2(int freeVolume, const vecSize_t startPos = 0, int usedContainers = 0);


	private:
		std::vector<int> m_input{};
		int m_permutations{0};
		int m_permutationsPart2{0};
		int m_minContainers{std::numeric_limits<int>::max()};
	};



	InputData::InputData(std::ifstream&& input)
	{
		int buffer{};

		while (!input.eof()) {
			input >> buffer;
			m_input.push_back(buffer);
		}

		if(input.fail()) throw AOC::aocError("Invalid input. Please check input data.");
	}



	void InputData::permutate(int freeVolume, const vecSize_t startPos, int usedContainers)
	{
		if ((startPos >= m_input.size()) || (freeVolume <= 0)) {
			return;
		}

		const vecSize_t new_startPos{startPos + 1};
		permutate(freeVolume, new_startPos, usedContainers); // do not use this container

		++usedContainers;
		freeVolume -= m_input[startPos];
		permutate(freeVolume, new_startPos, usedContainers);
		if (freeVolume == 0) {
			++m_permutations;
			m_minContainers = std::min(m_minContainers, usedContainers);
		}
	}



	void InputData::permutatePart2(int freeVolume, const vecSize_t startPos, int usedContainers)
	{
		if ((usedContainers >= m_minContainers) || (startPos >= m_input.size()) || (freeVolume <= 0)) {
			return;
		}

		const vecSize_t new_startPos = startPos + 1;
		permutatePart2(freeVolume, new_startPos, usedContainers); // do not use this container

		++usedContainers;
		freeVolume -= m_input[startPos];
		permutatePart2(freeVolume, new_startPos, usedContainers);
		if (freeVolume == 0) {
			++m_permutationsPart2;
		}

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



namespace AOC::Y2015::D17 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d17}};
		//testPuzzle(io);

		InputData inputData{io.getInputFile()};

		inputData.permutate(eggnog_volume);
		io.printSolution(inputData.getPermutations(), 1638);
		inputData.permutatePart2(eggnog_volume);
		io.printSolution(inputData.getPermutationsPart2(), 17);
	}
}