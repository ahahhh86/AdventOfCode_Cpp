module;

#include <algorithm>
#include <fstream>
#include <vector>

module Puzzle2015:D14;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	struct ReindeerStats
	{
		int speed{};
		int flyTime{};
		int restTime{};
	};



	auto& operator>>(std::ifstream& in, ReindeerStats& deer)
	{
		// format example: Comet can fly 14 km/s for 10 seconds, but then must rest for 127 seconds.
		std::string str{};
		in >> str; // name
		in.ignore(std::ssize("can fly "));
		in >> deer.speed;
		in.ignore(std::ssize("km/s for "));
		in >> deer.flyTime;
		in.ignore(std::ssize("seconds, but then must rest for "));
		in >> deer.restTime;
		in.ignore(std::ssize("seconds.\n"));
		if (in.fail()) throw AOC::InvalidFileInput();

		return in;
	}
}



namespace { // Calculations
	class Reindeer
	{
	public:
		Reindeer(const ReindeerStats& stats);

		Reindeer& operator=(Reindeer&) = delete;

		auto fly();
		auto getDistance() const;
		auto getPoints() const;
		void awardPoint();

	private:
		const ReindeerStats m_stats;
		int m_flyTime;
		int m_restTime;
		int m_distance{0};
		int m_points{0};
	};



	Reindeer::Reindeer(const ReindeerStats& stats):
		m_stats{stats},
		m_flyTime{stats.flyTime},
		m_restTime{stats.restTime}
	{
		// do nothing
	}



	auto Reindeer::fly()
	{
		if (m_flyTime > 0) { // fly
			--m_flyTime;
			m_distance += m_stats.speed;
		} else { // rest
			--m_restTime;
			if (m_restTime == 0) {
				m_flyTime = m_stats.flyTime;
				m_restTime = m_stats.restTime;
			}
		}

		return m_distance;
	}



	auto Reindeer::getDistance() const
	{
		return m_distance;
	}



	auto Reindeer::getPoints() const
	{
		return m_points;
	}



	void Reindeer::awardPoint()
	{
		++m_points;
	}



	using StatVector = std::vector<ReindeerStats>;
	using DeerVector = std::vector<Reindeer>;

	auto createDeers(const StatVector& stats)
	{
		DeerVector result{};

		std::ranges::for_each(stats, [&](const auto& stat){
			result.push_back({stat});
		});

		return result;
	}



	void raceDeers(DeerVector& deers, int time)
	{
		for (int i{0}; i < time; ++i) {
			int maxDistance{-1};

			std::ranges::for_each(deers, [&](Reindeer& deer) {
				maxDistance = std::max(maxDistance, deer.fly());
			});

			std::ranges::for_each(deers, [&](Reindeer& deer) {
				if (deer.getDistance() == maxDistance) {
					deer.awardPoint();
				}
			});
		}
	}



	auto getResults(const DeerVector& deers)
	{
		std::pair result{-1, -1};

		std::ranges::for_each(deers, [&](const Reindeer& deer) {
			result.first = std::max(result.first, deer.getDistance());
			result.second = std::max(result.second, deer.getPoints());
		});

		return result;
	}


	auto getRaceResults(const StatVector& stats, int time)
	{
		auto deers{createDeers(stats)};
		raceDeers(deers, time);
		return getResults(deers);
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const StatVector stats{
				{14, 10, 127},
				{16, 11, 162},
			};

			io.startTests();

			const auto raceResult{getRaceResults(stats, 1000)};
			io.printTest(raceResult.first, 1120);
			io.printTest(raceResult.second, 689);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D14 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d14}};
		testPuzzle(io);

		constexpr int raceTime{2503};
		const auto raceResult{getRaceResults(io.readInputFile<ReindeerStats>(), raceTime)};

		io.printSolution(raceResult.first, 2640);
		io.printSolution(raceResult.second, 1102);
	}
}