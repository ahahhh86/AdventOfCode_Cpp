module;

#include <array>
#include <fstream>
#include <numeric>
#include <vector>

module Puzzle2015:D02;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	constexpr int invalidLength{0};


	struct PresentBox
	{
		int length{invalidLength};
		int width{invalidLength};
		int height{invalidLength};

		bool isValid() const;
	};



	bool PresentBox::isValid() const
	{
		return length > invalidLength && width > invalidLength && height > invalidLength;
	}



	auto& operator>>(std::ifstream& in, PresentBox& box)
	{
		// format 0x0x0\n
		in >> box.length;
		in.ignore(1);		// first x
		in >> box.width;
		in.ignore(1);		// second x
		in >> box.height;

		if (!box.isValid()) { throw AOC::InvalidFileInput{}; }

		return in;
	}
}



namespace { // Calculations
	auto calculateWrappingPaper(const PresentBox& box)
	{
		const std::array sides{
			box.length * box.width,
			box.width * box.height,
			box.height * box.length
		};

		const int minSide{std::ranges::min(sides)};
		const int sidesSum{2 * std::accumulate(sides.cbegin(), sides.cend(), 0)};

		return sidesSum + minSide;
	}



	auto calculateRibbon(const PresentBox& box)
	{
		const std::array perimeters{
			2 * (box.length + box.width),
			2 * (box.width + box.height),
			2 * (box.height + box.length)
		};

		const int minPerimeter{std::ranges::min(perimeters)};
		const int volume{box.length * box.width * box.height};

		return minPerimeter + volume;
	}



	auto accumulatePresents(const std::vector<PresentBox>& vec, int(*fcn)(const PresentBox&))
	{
		return std::accumulate(vec.cbegin(), vec.cend(), 0, [=](int sum, const PresentBox& box) {
			return sum + fcn(box);
		});
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			constexpr std::array presents{
				PresentBox{2, 3, 4},
				PresentBox{1, 1, 10},
			};

			io.startTests();

			io.printTest(calculateWrappingPaper(presents[0]), 58);
			io.printTest(calculateWrappingPaper(presents[1]), 43);

			io.printTest(calculateRibbon(presents[0]), 34);
			io.printTest(calculateRibbon(presents[1]), 14);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D02 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d02}};
		testPuzzle(io);

		const auto presents{io.readInputFile<PresentBox>()};
		io.printSolution(accumulatePresents(presents, calculateWrappingPaper), 1598415);
		io.printSolution(accumulatePresents(presents, calculateRibbon), 3812909);
	}
}