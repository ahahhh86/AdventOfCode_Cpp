module;

#include <algorithm>
//#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D16;

import BasicImports;
import Point2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class ContraptionPart
	{
		empty,
		mirrorNtoE, // = StoW
		mirrorNtoW, // = StoE
		splitterNS,
		splitterEW,
	};




	auto charToContraptionPart(char c)
	{
		switch (c) {
		case '.': return ContraptionPart::empty;
		case '/': return ContraptionPart::mirrorNtoE;
		case '\\': return ContraptionPart::mirrorNtoW;
		case '|': return ContraptionPart::splitterNS;
		case '-': return ContraptionPart::splitterEW;
		default: throw AOC::InvalidFileInput();
		}
	}



	using Contraption = std::vector<std::vector<ContraptionPart>>;
	auto readContraption(std::vector<std::string>&& strs)
	{
		Contraption result{};
		result.reserve(strs.size());

		int index{1};
		std::ranges::for_each(strs, [&](std::string_view str) {
			if (str.size() != strs[0].size()) throw AOC::InvalidFileLine(index);

			std::vector<ContraptionPart> buffer{};
			buffer.reserve(str.size());

			std::ranges::for_each(str, [&](char c) {
				buffer.push_back(charToContraptionPart(c));
			});

			result.push_back(buffer);
			++index;
		});

		return result;
	}
}



namespace { // Calculations
	enum class Direction
	{
		north,
		east,
		south,
		west,
	};

	using Point = AOC::Point2D<int>;

	struct Beam
	{
		Point pos{0, 0};
		Direction dir{Direction::east};
	};

	bool operator==(const Beam& b1, const Beam& b2)
	{
		return b1.dir == b2.dir && b1.pos == b2.pos;
	}

	constexpr Point north{-1, 0};
	constexpr Point east{0, 1};
	constexpr Point south{1, 0};
	constexpr Point west{0, -1};

	using Beams = std::vector<Beam>;

	void setNextBeamEmpty(Beam& beam)
	{
		switch (beam.dir) {
		case Direction::north:
			beam.pos += north;
			break;
		case Direction::east:
			beam.pos += east;
			break;
		case Direction::south:
			beam.pos += south;
			break;
		case Direction::west:
			beam.pos += west;
			break;
		}
	}

	void setNextBeamMirrorNtoE(Beam& beam) // '/'
	{
		switch (beam.dir) {
		case Direction::north:
			beam.pos += east;
			beam.dir = Direction::east;
			break;
		case Direction::east:
			beam.pos += north;
			beam.dir = Direction::north;
			break;
		case Direction::south:
			beam.pos += west;
			beam.dir = Direction::west;
			break;
		case Direction::west:
			beam.pos += south;
			beam.dir = Direction::south;
			break;
		}
	}

	void setNextBeamMirrorNtoW(Beam& beam) // '\'
	{
		switch (beam.dir) {
		case Direction::north:
			beam.pos += west;
			beam.dir = Direction::west;
			break;
		case Direction::east:
			beam.pos += south;
			beam.dir = Direction::south;
			break;
		case Direction::south:
			beam.pos += east;
			beam.dir = Direction::east;
			break;
		case Direction::west:
			beam.pos += north;
			beam.dir = Direction::north;
			break;
		}
	}

	void setNextBeamSplitterNS(Beams& beam)
	{
		switch (beam[0].dir) {
		case Direction::north:
			beam[0].pos += north;
			break;
		case Direction::south:
			beam[0].pos += south;
			break;
		case Direction::east:
		case Direction::west:
			beam.push_back(beam[0]);
			beam[0].pos += north;
			beam[0].dir = Direction::north;
			beam[1].pos += south;
			beam[1].dir = Direction::south;
			break;
		}
	}

	void setNextBeamSplitterEW(Beams& beam)
	{
		switch (beam[0].dir) {
		case Direction::east:
			beam[0].pos += east;
			break;
		case Direction::west:
			beam[0].pos += west;
			break;
		case Direction::north:
		case Direction::south:
			beam.push_back(beam[0]);
			beam[0].pos += east;
			beam[0].dir = Direction::east;
			beam[1].pos += west;
			beam[1].dir = Direction::west;
			break;
		}
	}

	bool isInBounds(Point pos, int sizeX, int sizeY)
	{
		return 0 <= pos.x && pos.x < sizeX
			&& 0 <= pos.y && pos.y < sizeY;
	}

	auto getNextBeamPos(const Beam& beam, const Contraption& contraption)
	{
		Beams result{beam};

		switch (contraption[beam.pos.x][beam.pos.y]) {
		case ContraptionPart::empty:
			setNextBeamEmpty(result[0]);
			break;
		case ContraptionPart::mirrorNtoE:
			setNextBeamMirrorNtoE(result[0]);
			break;
		case ContraptionPart::mirrorNtoW:
			setNextBeamMirrorNtoW(result[0]);
			break;
		case ContraptionPart::splitterNS:
			setNextBeamSplitterNS(result);
			break;
		case ContraptionPart::splitterEW:
			setNextBeamSplitterEW(result);
			break;
		}

		std::erase_if(result, [&](const Beam& b) {
			return !isInBounds(b.pos, contraption.size(), contraption[0].size());
		});

		return result;
	}

	void removeDouble(Beams& beam, const Beams& energized)
	{
		std::erase_if(beam, [&](const Beam& b){
			return std::find(energized.cbegin(), energized.cend(), b) != energized.cend();
		});
	}

	auto calculateEnergized(const Contraption& contraption, const Beam& start = {})
	{
		Beams beams{start};
		Beams energized{beams};

		do {
			Beams buffer{};

			std::ranges::for_each(beams, [&](const Beam& beam) {
				Beams b{getNextBeamPos(beam, contraption)};
				removeDouble(b, energized);
				buffer.append_range(b);
			});

			energized.append_range(buffer);
			beams = buffer;
		} while (beams.size() > 0);

		// if a beam heads in different directions, the position in energized is doubled
		// use std::set to meke thm unique
		std::set<Point> result{};
		std::ranges::for_each(energized, [&](const Beam& b) {
			result.insert(b.pos);
		});
		return static_cast<int>(result.size());
	}

	auto getMaxEnergized(const Contraption& contraption)
	{
		int result{-1};

		const int maxPos{static_cast<int>(contraption[0].size()) - 1};
		for (int i{0}; i <= maxPos; ++i) {
			result = std::max({result,
							  calculateEnergized(contraption, {{0, i}, Direction::south}),
							  calculateEnergized(contraption, {{maxPos, i}, Direction::north})});
		}

		const int maxLine{static_cast<int>(contraption.size()) - 1};
		for (int i{0}; i <= maxLine; ++i) {
			result = std::max({result,
							  calculateEnergized(contraption, {{i, 0}, Direction::east}),
							  calculateEnergized(contraption, {{i, maxLine}, Direction::west})});
		}

		return result;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const Contraption contraption{readContraption({
				".|...\\....",
				"|.-.\\.....",
				".....|-...",
				"........|.",
				"..........",
				".........\\",
				"..../.\\\\..",
				".-.-/..|..",
				".|....-|.\\",
				"..//.|....",
			})};

			io.startTests();

			io.printTest(calculateEnergized(contraption), 46);
			io.printTest(getMaxEnergized(contraption), 51);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D16 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d16}};
		testPuzzle(io);

		const auto contraption{readContraption(io.readInputFile<std::string>())};
		io.printSolution(calculateEnergized(contraption), 7472);
		io.printSolution(getMaxEnergized(contraption), 7716);
	}
}