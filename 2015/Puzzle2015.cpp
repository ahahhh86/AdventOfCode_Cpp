module;

#include <print>

module Puzzle2015;

import AdventDate;
import :D01;
import :D02;
import :D03;
import :D04;
import :D05;
import :D06;
import :D07;
import :D08;
import :D09;
import :D10;
import :D11;
import :D12;
import :D13;
import :D14;
/*
import :D15;
import :D16;
import :D17;
import :D18;
import :D19;
import :D20;
import :D21;
import :D22;
import :D23;
import :D24;
import :D25;
*/



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC::Y2015 {
	void solvePuzzle(Day day)
	{
		switch (day) {
		case Day::d01: D01::solvePuzzle(); break;
		case Day::d02: D02::solvePuzzle(); break;
		case Day::d03: D03::solvePuzzle(); break;
		case Day::d04: D04::solvePuzzle(); break;
		case Day::d05: D05::solvePuzzle(); break;
		case Day::d06: D06::solvePuzzle(); break;
		case Day::d07: D07::solvePuzzle(); break;
		case Day::d08: D08::solvePuzzle(); break;
		case Day::d09: D09::solvePuzzle(); break;
		case Day::d10: D10::solvePuzzle(); break;
		case Day::d11: D11::solvePuzzle(); break;
		case Day::d12: D12::solvePuzzle(); break;
		case Day::d13: D13::solvePuzzle(); break;
		case Day::d14: D14::solvePuzzle(); break;
		/*
		case Day::d15: D15::solvePuzzle(); break;
		case Day::d16: D16::solvePuzzle(); break;
		case Day::d17: D17::solvePuzzle(); break;
		case Day::d18: D18::solvePuzzle(); break;
		case Day::d19: D19::solvePuzzle(); break;
		case Day::d20: D20::solvePuzzle(); break;
		case Day::d21: D21::solvePuzzle(); break;
		case Day::d22: D22::solvePuzzle(); break;
		case Day::d23: D23::solvePuzzle(); break;
		case Day::d24: D24::solvePuzzle(); break;
		case Day::d25: D25::solvePuzzle(); break;
		*/
		default:
			printDate({Year::y2015, day});
			std::println("This puzzle has not been solved.");
			break;
#pragma warning(suppress : 4061) // C4061 enumerator in switch of enum is not explicitly handled by a case label
		}
	}
}