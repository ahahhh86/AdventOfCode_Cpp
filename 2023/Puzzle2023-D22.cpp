module;

#include <algorithm>
#include <fstream>
#include <vector>

module Puzzle2023:D22;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	constexpr int groundLevel{0};
	constexpr int lowestBrickLevel{groundLevel + 1};

	struct Position
	{
		int x{0};
		int y{0};
		int z{0};
	};
	using PositionVector = std::vector<Position>;

	// if we sort the bricks with lowest first, it is probably faster when they all fall
	// so only z is considered
	bool operator<(const Position& p1, const Position& p2)
	{
		return p1.z < p2.z;
	}

	bool operator==(const Position& p1, const Position& p2)
	{
		return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
	}


	auto& operator>>(std::ifstream& in, Position& pos)
	{
		char buffer{};

		in >> pos.x;
		if (pos.x < 0) { throw AOC::InvalidFileInput{}; }

		in >> buffer;
		if (buffer != ',') { throw AOC::InvalidFileInput{}; }
		in >> pos.y;
		if (pos.y < 0) { throw AOC::InvalidFileInput{}; }

		in >> buffer;
		if (buffer != ',') { throw AOC::InvalidFileInput{}; }
		in >> pos.z;
		if (pos.z < lowestBrickLevel) { throw AOC::InvalidFileInput{}; }

		return in;
	}



	using BrickVecktor = std::vector<PositionVector>;

	void fall(PositionVector& brick)
	{
		std::ranges::for_each(brick, [&](Position& i) {
			--i.z;
		});
	}



	// if we sort the bricks with lowest first, it is probably faster when they all fall
	// so only z is considered
	bool operator<(const PositionVector& b1, const PositionVector& b2)
	{
		return b1[0] < b2[0];
	}



	// the first Position should have the lowest value
	auto createPositionVector(Position p1, Position p2)
	{
		PositionVector result{};

		if (p1.x != p2.x) {
			if (p1.y != p2.y || p1.z != p2.z) { throw AOC::InvalidFileInput{}; }
			if (p1.x > p2.x) { std::swap(p1.x, p2.x); }

			for (int i{p1.x}; i <= p2.x; ++i) {
				result.push_back({i, p1.y, p1.z});
			}
		} else if (p1.y != p2.y) {
			if (p1.x != p2.x || p1.z != p2.z) { throw AOC::InvalidFileInput{}; }
			if (p1.y > p2.y) { std::swap(p1.y, p2.y); }

			for (int i{p1.y}; i <= p2.y; ++i) {
				result.push_back({p1.x, i, p1.z});
			}
		} else if(p1.z != p2.z) {
			if (p1.y != p2.y || p1.x != p2.x) { throw AOC::InvalidFileInput{}; }
			if (p1.z > p2.z) { std::swap(p1.z, p2.z); }

			for (int i{p1.z}; i <= p2.z; ++i) {
				result.push_back({p1.x, p1.y, i});
			}
		} else {
			result.push_back({p1.x, p1.y, p1.z});
		}

		return result;
	}



	auto& operator>>(std::ifstream& in, PositionVector& brick)
	{
		Position start{};
		in >> start;

		char buffer{};
		in >> buffer;
		if (buffer != '~') { throw AOC::InvalidFileInput{}; }

		Position end{};
		in >> end;

		brick = createPositionVector(start, end);

		return in;
	}
}



namespace { // Calculations
	bool doBricksIntersect(const PositionVector& b1, const PositionVector& b2)
	{
		for (const Position& p1 : b1) {
			if (std::ranges::find(b2, p1) != b2.cend()) { return true; }
		}

		return false;
	}



	bool canFall(const BrickVecktor::const_iterator brick, const BrickVecktor& vector)
	{
		if ((*brick)[0].z <= lowestBrickLevel) { return false; }	// pos[0] should always have the lowest value

		PositionVector newPos{*brick};
		fall(newPos);

		for (auto i{vector.cbegin()}; i != vector.cend(); ++i) {
			if (i == brick) { continue; }

			if (doBricksIntersect(newPos, *i)) { return false; }
		}

		return true;
	}



	void letBricksFall(BrickVecktor& bricks)
	{
		int fallenCount{};

		do {
			fallenCount = 0;
			for (auto i{bricks.begin()}; i != bricks.end(); ++i) {
				if (canFall(i, bricks)) {
					fall(*i);
					++fallenCount;
				}
			}
		} while (fallenCount > 0);
	}



	//bool canDesintegrated(const BrickVecktor::const_iterator brick, const BrickVecktor& vector)
	//{ // TODO:
	//}



	//auto countDesintegratable(const BrickVecktor& bricks)
	//{
	//	int desintegrated{0};

	//	for (auto i{bricks.cbegin()}; i != bricks.cend(); ++i) {
	//		if (canDesintegrated(i, bricks)) {
	//			++desintegrated;
	//		}
	//	}

	//	return desintegrated;
	//}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			BrickVecktor test{
				{createPositionVector({1,0,1}, {1,2,1})},
				{createPositionVector({0,0,2}, {2,0,2})},
				{createPositionVector({0,2,3}, {2,2,3})},
				{createPositionVector({0,0,4}, {0,2,4})},
				{createPositionVector({2,0,5}, {2,2,5})},
				{createPositionVector({0,1,6}, {2,1,6})},
				{createPositionVector({1,1,8}, {1,1,9})},
			};

			io.startTests();

			letBricksFall(test);
			io.printTest(test[0][0].z, 1);
			io.printTest(test[1][0].z, 2);
			io.printTest(test[2][0].z, 2);
			io.printTest(test[3][0].z, 3);
			io.printTest(test[4][0].z, 3);
			io.printTest(test[5][0].z, 4);
			io.printTest(test[6][0].z, 5);
			io.printMsg("");

			//io.printTest(countDesintegratable(test), 5);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D22 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d22}};
		testPuzzle(io);

		auto bricks{io.readInputFile<PositionVector>()};
		//std::sort(bricks.begin(), bricks.end());
		//letBricksFall(bricks); // TODO: slow
		io.printSolution(0, 0);
	}
}