module;

#include <limits>
#include <string>
#include <vector>

module Puzzle2015:D17;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Calculations
	using ContainerVector = std::vector<int>;

	class Containers
	{
	public:
		explicit Containers(ContainerVector&& containers);

		auto countCombinationsPart1(int freeVolume);
		auto countCombinationsPart2(int freeVolume);

	private:
		void getCombinationCountPart1(int freeVolume, std::size_t startPos, int usedContainers);
		void getCombinationCountPart2(int freeVolume, std::size_t startPos, int usedContainers);

		ContainerVector m_containers{};
		std::pair<int, int> m_combinationCount{0, 0};
		int m_minContainers{std::numeric_limits<int>::max()};
	};



	Containers::Containers(ContainerVector&& containers):
		m_containers{containers}
	{
		// do nothing
	}



	auto Containers::countCombinationsPart1(int freeVolume)
	{
		getCombinationCountPart1(freeVolume, 0, 0);
		return m_combinationCount.first;
	}



	auto Containers::countCombinationsPart2(int freeVolume)
	{
		if (m_minContainers == std::numeric_limits<int>::max()) {
			getCombinationCountPart1(freeVolume, 0, 0);
		}

		getCombinationCountPart2(freeVolume, 0, 0);
		return m_combinationCount.second;
	}



	void Containers::getCombinationCountPart1(int freeVolume, std::size_t startPos, int usedContainers)
	{
		if ((startPos >= m_containers.size()) || (freeVolume <= 0)) {
			return;
		}

		const auto newStartPos{startPos + 1};

		// try combinations without the container at startPos
		getCombinationCountPart1(freeVolume, newStartPos, usedContainers);

		// try combinations including the container at startPos
		++usedContainers;
		freeVolume -= m_containers[startPos];
		getCombinationCountPart1(freeVolume, newStartPos, usedContainers);

		if (freeVolume == 0) {
			++m_combinationCount.first;
			m_minContainers = std::min(m_minContainers, usedContainers);
		}
	}



	void Containers::getCombinationCountPart2(int freeVolume, std::size_t startPos, int usedContainers)
	{
		if ((usedContainers >= m_minContainers) || (startPos >= m_containers.size()) || (freeVolume <= 0)) {
			return;
		}

		const auto newStartPos{startPos + 1};

		// try combinations without the container at startPos
		getCombinationCountPart2(freeVolume, newStartPos, usedContainers);

		// try combinations including the container at startPos
		++usedContainers;
		freeVolume -= m_containers[startPos];
		getCombinationCountPart2(freeVolume, newStartPos, usedContainers);

		if (freeVolume == 0) {
			++m_combinationCount.second;
		}
	}
}


namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			constexpr int eggnoggVolume{25};
			Containers canisters{{20, 15, 10, 5, 5}};

			io.startTests();

			io.printTest(canisters.countCombinationsPart1(eggnoggVolume), 4);
			io.printTest(canisters.countCombinationsPart2(eggnoggVolume), 3);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D17 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d17}};
		testPuzzle(io);

		constexpr int eggnoggVolume{150};
		Containers canisters{io.readInputFile<int>()};

		io.printSolution(canisters.countCombinationsPart1(eggnoggVolume), 1638);
		io.printSolution(canisters.countCombinationsPart2(eggnoggVolume), 17);
	}
}