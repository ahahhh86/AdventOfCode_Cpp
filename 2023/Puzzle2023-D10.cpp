module;

#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>

module Puzzle2023:D10;

import BasicImports;
import Point2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Tile
	{
		start,
		northSouth,
		eastWest,
		northEast,
		northWest,
		southWest,
		southEast,
		ground,
	};



	auto charToTile(char c)
	{
		switch (c) {
		case'S': return Tile::start;
		case'|': return Tile::northSouth;
		case'-': return Tile::eastWest;
		case'L': return Tile::northEast;
		case'J': return Tile::northWest;
		case'7': return Tile::southWest;
		case'F': return Tile::southEast;
		case'.': return Tile::ground;
		default: throw AOC::InvalidFileInput{};
		}
	}



	auto& operator>>(std::stringstream& in, Tile& tile)
	{
		char buffer{};
		in >> buffer;
		if (!in.eof()) {tile = charToTile(buffer);}
		return in;
	}



	using SketchLine = std::vector<Tile>;

	auto readLine(std::stringstream&& in)
	{
		SketchLine result{};

		Tile tile{};
		in >> tile;
		while (!in.eof()) {
			result.push_back(tile);
			in >> tile;
		}
		return result;
	}



	auto& operator>>(std::ifstream& in, SketchLine& line)
	{
		std::string buffer{};
		std::getline(in, buffer);
		line = readLine(std::stringstream{buffer});
		return in;
	}



	using Sketch = std::vector<SketchLine>;
	void validate(const Sketch& sketch)
	{
		const auto differentLineLength{std::ranges::count_if(sketch, [&](const SketchLine& line) {
			return line.size() != sketch[0].size();
		})};

		if (differentLineLength != 0) { throw AOC::InvalidInputData{"validate"}; }
	}
}



namespace { // Calculations
	using Point = AOC::Point2D<int>;
	using PointVec = std::vector<Point>;



	auto getStartingPos(const Sketch& sketch)
	{
		PointVec startPos{};

		for (std::size_t line{0}; line < sketch.size(); ++line) {
			for (std::size_t pos{0}; pos < sketch[line].size(); ++pos) {
				if (sketch[line][pos] == Tile::start) {
					startPos.push_back({static_cast<int>(line), static_cast<int>(pos)});
				}
			}
		}

		if (startPos.size() != 1) { throw AOC::InvalidInputData{"getStartingPos()"}; }
		return startPos[0];
	}



	constexpr Point north{-1, 0};
	constexpr Point east{0, 1};
	constexpr Point south{1, 0};
	constexpr Point west{0, -1};



	bool isValidTile(Point nextPos, Point startPos, const Sketch& sketch)
	{
#pragma warning(disable : 4365) // C4365: signed/unsigned mismatch
#pragma warning(disable : 4388) // C4388: >= signed/unsigned mismatch

		if (nextPos.x < 0
		 || nextPos.y < 0
		 || nextPos.x >= sketch.size()
		 || nextPos.y >= sketch[nextPos.x].size()) {
			return false;
		}

		const Tile nextTile{sketch[nextPos.x][nextPos.y]};

#pragma warning(default : 4388)
#pragma warning(default : 4365)

		switch (nextTile) {
		case Tile::start:
		case Tile::ground: return false;
		}

		const auto direction{nextPos - startPos};
		if (direction == north) {
			switch (nextTile) {
			case Tile::eastWest:
			case Tile::northEast:
			case Tile::northWest:return false;
			case Tile::northSouth:
			case Tile::southWest:
			case Tile::southEast:return true;
			}
		} else if (direction == east) {
			switch (nextTile) {
			case Tile::southEast:
			case Tile::northSouth:
			case Tile::northEast:return false;
			case Tile::eastWest:
			case Tile::northWest:
			case Tile::southWest:return true;
			}
		} else if (direction == south) {
			switch (nextTile) {
			case Tile::eastWest:
			case Tile::southWest:
			case Tile::southEast:return false;
			case Tile::northEast:
			case Tile::northWest:
			case Tile::northSouth:return true;
			}
		} else if (direction == west) {
			switch (nextTile) {
			case Tile::northWest:
			case Tile::southWest:
			case Tile::northSouth:return false;
			case Tile::southEast:
			case Tile::northEast:
			case Tile::eastWest:return true;
			}
		} else {
			throw AOC::InvalidInputData{"isValidTile"};
		}

		return false; // should not be reached, but to prevent compiler warning
	}



	auto getFirstStep(const Sketch& sketch, Point startPos)
	{
		PointVec pos{};

		Point newPos{startPos + north};
		if (isValidTile(newPos, startPos, sketch)) {
			pos.push_back(newPos);
		}

		newPos = startPos + east;
		if (isValidTile(newPos, startPos, sketch)) {
			pos.push_back(newPos);
		}

		newPos = startPos + south;
		if (isValidTile(newPos, startPos, sketch)) {
			pos.push_back(newPos);
		}

		newPos = startPos + west;
		if (isValidTile(newPos, startPos, sketch)) {
			pos.push_back(newPos);
		}

		if (pos.size() != 2) { throw AOC::InvalidInputData{"getFirstStep()"}; }
		return pos;
	}



	auto chooseNewPos(Point p1, Point p2, Point oldPos)
	{
		if (p1 == oldPos) {
			return p2;
		} else if (p2 == oldPos) {
			return p1;
		} else {
			throw AOC::InvalidInputData{"chooseNewPos()"};
		}
	}



	auto getNextPos(Point oldPos, Point pos, Tile tile)
	{
		switch (tile) {
		case Tile::eastWest: return chooseNewPos(pos + east, pos + west, oldPos);
		case Tile::northEast: return chooseNewPos(pos + north, pos + east, oldPos);
		case Tile::northWest: return chooseNewPos(pos + north, pos + west, oldPos);
		case Tile::northSouth: return chooseNewPos(pos + north, pos + south, oldPos);
		case Tile::southWest: return chooseNewPos(pos + south, pos + west, oldPos);
		case Tile::southEast: return chooseNewPos(pos + south, pos + east, oldPos);
		default: throw AOC::InvalidInputData{"getNextPos"};
		}
	}



	void setNextPos(Point& oldPos, Point& pos, const Sketch& sketch)
	{
#pragma warning(suppress : 4365) // C4365: signed/unsigned mismatch
		const auto nextPos{getNextPos(oldPos, pos, sketch[pos.x][pos.y])};
		oldPos = pos;
		pos = nextPos;
	}



	using Loop = std::set<Point>;
	auto getLoop(const Sketch& sketch)
	{
		const Point startPos{getStartingPos(sketch)};
		PointVec oldPosVec{startPos, startPos};
		PointVec posVec{getFirstStep(sketch, startPos)};
		Loop loop{startPos, posVec[0], posVec[1]};

		do {
			setNextPos(oldPosVec[0], posVec[0], sketch);
			setNextPos(oldPosVec[1], posVec[1], sketch);

			loop.insert(posVec[0]);
			loop.insert(posVec[1]);
		} while (posVec[0] != posVec[1]);

		return loop;
	}



	auto countStepsToFarthestTile(const Loop& loop)
	{
		return static_cast<int>(loop.size() / 2);
	}



	// TODO: Part2
	//enum class EnclosedTile
	//{
	//	Unknown,
	//	Outside,
	//	Loop,
	//	Enclosed,
	//};

	//using EnclosedLine = std::vector<EnclosedTile>;
	//using EnclosedSketch = std::vector<EnclosedLine>;



	//// TODO: Refactoring
	//auto getEnclosedSketch(const Sketch& sketch, const Loop& loop)
	//{
	//	EnclosedSketch result{};
	//	EnclosedLine line(sketch.size());
	//	std::fill(line.begin(), line.end(), EnclosedTile::Unknown);
	//	for (int i{0}; i < sketch.size(); ++i) {
	//		result.push_back(line);
	//	};
	//	const auto lastLine{static_cast<int>(sketch.size()) - 1};
	//	const auto lastRow{static_cast<int>(sketch[0].size()) - 1};



	//	// First and last line can not be enclosed
	//	for (auto i{0}; i < result[0].size(); ++i) {
	//		if (loop.find({0, i}) != loop.cend()) {
	//			result[0][i] = EnclosedTile::Loop;
	//		} else {
	//			result[0][i] = EnclosedTile::Outside;
	//		}

	//		if (loop.find({lastLine, i}) != loop.cend()) {
	//			result[lastLine][i] = EnclosedTile::Loop;
	//		} else {
	//			result[lastLine][i] = EnclosedTile::Outside;
	//		}
	//	}

	//	// First and last column can not be enclosed
	//	// First and last line already covered
	//	for (auto i{1}; i < result.size() - 1; ++i) {
	//		if (loop.find({i, 0}) != loop.cend()) {
	//			result[i][0] = EnclosedTile::Loop;
	//		} else {
	//			result[i][0] = EnclosedTile::Outside;
	//		}

	//		if (loop.find({i, lastRow}) != loop.cend()) {
	//			result[i][lastRow] = EnclosedTile::Loop;
	//		} else {
	//			result[i][lastRow] = EnclosedTile::Outside;
	//		}
	//	}

	//	for (auto i{1}; i < result.size() - 1; ++i) {
	//		for (auto j{1}; j < result[0].size() - 1; ++j) {
	//			if (loop.find({i, j}) != loop.cend()) {
	//				result[i][j] = EnclosedTile::Loop;
	//			} else {
	//				// TODO:
	//				// if adjacent Outside -> self Outside
	//				// if adjacent Enclosed -> self Enclosed
	//				// Problem: open loop
	//			}
	//		}
	//	}

	//	return result;
	//}



	//auto countEnclosed(const EnclosedSketch& sketch)
	//{
	//	std::size_t result{0};

	//	std::ranges::for_each(sketch, [&](const EnclosedLine& line) {
	//		result += std::ranges::count_if(line, [](EnclosedTile i){
	//			return i == EnclosedTile::Enclosed;
	//		});
	//	});

	//	return result;
	//}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			Sketch test1{};
			test1.push_back(readLine(std::stringstream{"-L|F7"}));
			test1.push_back(readLine(std::stringstream{"7S-7|"}));
			test1.push_back(readLine(std::stringstream{"L|7||"}));
			test1.push_back(readLine(std::stringstream{"-L-J|"}));
			test1.push_back(readLine(std::stringstream{"L|-JF"}));
			validate(test1);

			Sketch test2{};
			test2.push_back(readLine(std::stringstream{"7-F7-"}));
			test2.push_back(readLine(std::stringstream{".FJ|7"}));
			test2.push_back(readLine(std::stringstream{"SJLL7"}));
			test2.push_back(readLine(std::stringstream{"|F--J"}));
			test2.push_back(readLine(std::stringstream{"LJ.LJ"}));
			validate(test2);

			io.startTests();

			Loop loop1{getLoop(test1)};
			io.printTest(countStepsToFarthestTile(loop1), 4);

			Loop loop2{getLoop(test2)};
			io.printTest(countStepsToFarthestTile(loop2), 8);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D10 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d10}};
		testPuzzle(io);

		const auto input{io.readInputFile<SketchLine>()};
		validate(input);
		const auto loop{getLoop(input)};
		io.printSolution(countStepsToFarthestTile(loop), 6815);
		//io.printSolution(countEnclosed(getEnclosedSketch(sketch, loop)), 0) // TODO:
	}
}