/* This module delegates the solving of a puzzle to the appropriate mudule. */

module;

#include <print>
//#include <stdexcept>

module SolvePuzzle;

//import std; // TODO: import if IntelliSense works with it
import Errors;
import AdventDate;
import Puzzle2015;
import Puzzle2023;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC {
	void solvePuzzle(Date d)
	{
		try {
			switch (d.year) {
			case Year::y2015: Y2015::solvePuzzle(d.day); break;
			/*
			case Year::y2016: Y2016::solvePuzzle(d.day); break;
			case Year::y2017: Y2017::solvePuzzle(d.day); break;
			case Year::y2018: Y2018::solvePuzzle(d.day); break;
			case Year::y2019: Y2019::solvePuzzle(d.day); break;
			case Year::y2020: Y2020::solvePuzzle(d.day); break;
			case Year::y2021: Y2021::solvePuzzle(d.day); break;
			case Year::y2022: Y2022::solvePuzzle(d.day); break;
			*/
			case Year::y2023: Y2023::solvePuzzle(d.day); break;

			default:
				printDate(d);
				std::println("This puzzle has not been solved.");
				break;
#pragma warning(suppress : 4061) // C4061 enumerator in switch of enum is not explicitly handled by a case label
			}

		} catch (const aocError& e) {
			std::println("An error occured: {}!", e.what());
		} catch (const std::exception& e) {
			std::println("An unexpected error occured: {}!", e.what());
		} catch (...) {
			std::println("An unknown error occured!");
		}
	}
}