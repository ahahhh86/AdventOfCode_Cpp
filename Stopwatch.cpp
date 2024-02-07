module;

#include <chrono>

module Stopwatch;

//import std; // TODO: import if IntelliSense works with it


// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC {
		double Stopwatch::getTime() const
		{
			return std::chrono::duration_cast<Second>(Clock::now() - m_startTime).count();
		}



		void Stopwatch::reset()
		{
			m_startTime = Clock::now();
		}
}