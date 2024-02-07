/* This module contains a class to check how long something takes. */

module;

#include <chrono>

export module Stopwatch;

//import std; // TODO: import if IntelliSense works with it


// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC {
	export class Stopwatch
	{
	public:
		double getTime() const;		// gets the time [s] since creation or reset
		void reset();				// resets the start time

	private:
		using Clock = std::chrono::steady_clock;
		using Second = std::chrono::duration<double, std::ratio<1>>;

		std::chrono::time_point<Clock> m_startTime{Clock::now()};
	};
}