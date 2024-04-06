module;

#include <algorithm>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

module Puzzle2023:D13;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	// TODO: #. to enum class
	using Pattern = std::vector<std::string>;
	using Notes = std::vector<Pattern>;

	auto& operator>>(std::ifstream& in, Pattern& pattern)
	{
		pattern = {};
		while (true) {
			std::string buffer{};
			std::getline(in, buffer);
			if (buffer == "") {
				break;
			} else {
				pattern.push_back(buffer);
			}
		}

		return in;
	}



	bool isValid(const Pattern& pattern)
	{
		for (std::string_view str : pattern) {
			if (str.size() != pattern[0].size()) {return false;}
			if (str.find_first_not_of("#.") != std::string_view::npos) { return false; }
		}

		return true;
	}



	void validate(const Notes& notes)
	{
		std::size_t index{1};
		std::ranges::for_each(notes, [&](const Pattern& p) {
			if (!isValid(p)) { throw AOC::InvalidFileLine{index}; }
			++index;
		});
	}
}



namespace { // Calculations
	bool isReflectiveLine(const Pattern& pattern, std::size_t index, std::size_t offset = 0) // TODO: non-recursive version?
	{
		if (offset > index || pattern.size() <= index + offset + 1) { throw AOC::InvalidInputData{"isReflectiveLine()"}; }

		const auto firstLine{index - offset};
		const auto lastLine{index + offset + 1};

		if (pattern[firstLine] != pattern[lastLine]) {
			return false;
		}

		if (firstLine == 0 || lastLine + 1 == pattern.size()) {
			return true;
		}

		return isReflectiveLine(pattern, index, offset + 1);
	}



	using Exclude = std::optional<int>;
	auto findReflectiveRow(const Pattern& pattern, Exclude exclude = {})
	{
		const auto loopEnd{pattern.size() - 1};
		for (std::size_t i{0}; i < loopEnd; ++i) {
			const auto index{static_cast<int>(i) + 1};

			if (exclude && *exclude == index) {continue;}
			if (isReflectiveLine(pattern, i)) {return index;}
		}

		return 0;
	}



	auto getTransposedPattern(const Pattern& pattern)
	{
		Pattern result{};
		result.reserve(pattern[0].size());

		for (std::size_t pos{0}; pos < pattern[0].size(); ++pos) {
			std::string buffer{};
			buffer.reserve(pattern.size());
			for (std::size_t line{0}; line < pattern.size(); ++line) {
				buffer.push_back(pattern[line][pos]);
			}
			result.push_back(buffer);
		}

		return result;
	}



	auto findReflectiveCol(const Pattern& pattern, Exclude exclude = {})
	{
		return findReflectiveRow(getTransposedPattern(pattern), exclude);
	}



	auto summarizePattern(const Pattern& pattern, Exclude exclude = {})
	{
		constexpr int rowFactor{100};
		int colCount{};
		int rowCount{};

		if (exclude) {
			if (*exclude < rowFactor) {
				colCount = findReflectiveCol(pattern, exclude);
				rowCount = findReflectiveRow(pattern);
			} else {
				colCount = findReflectiveCol(pattern);
				rowCount = findReflectiveRow(pattern, *exclude / rowFactor);

			}
		} else {
			colCount = findReflectiveCol(pattern);
			rowCount = findReflectiveRow(pattern);
		}
		

		return colCount + rowCount * rowFactor;
	}



	auto accumulatePatterns(const Notes& notes, int(*fcn)(const Pattern&, Exclude))
	{
		return std::accumulate(notes.cbegin(), notes.cend(), 0, [=](auto sum, const Pattern& pattern) {
			return sum + fcn(pattern, {});
		});
	}



	void setSmudge(char& c)
	{
		if (c == '.') {
			c = '#';
		} else {
			c = '.';
		}
	}



	// parameter "Exclude" to make compatible with accumulatePatterns()
	auto summarizePatternWithSmudge(const Pattern& p, Exclude)
	{
		// copied because nedds to change and needs same parameters as summarizePattern(const Pattern&)
		Pattern pattern{p};
		const auto oldValue{summarizePattern(p)};

		for (std::size_t i{0}; i < p.size(); ++i) {
			for (std::size_t j{0}; j < p[0].size(); ++j) {
				setSmudge(pattern[i][j]);
				const auto patternValue{summarizePattern(pattern, oldValue)};
				if (patternValue != 0) {
					return patternValue;
				}
				setSmudge(pattern[i][j]);
			}
		}

		throw AOC::InvalidInputData{"summarizePatternWithSmudge()"};
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const Notes notes{
				{
					"#.##..##.",
					"..#.##.#.",
					"##......#",
					"##......#",
					"..#.##.#.",
					"..##..##.",
					"#.#.##.#.",
				},{
					"#...##..#",
					"#....#..#",
					"..##..###",
					"#####.##.",
					"#####.##.",
					"..##..###",
					"#....#..#",
				}};

			io.startTests();

			validate(notes);

			io.printTest(findReflectiveRow(notes[1]), 4);
			io.printTest(findReflectiveRow(getTransposedPattern(notes[0])), 5);

			io.printTest(accumulatePatterns(notes, summarizePattern), 405);
			io.printTest(accumulatePatterns(notes, summarizePatternWithSmudge), 400);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D13 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d13}};
		testPuzzle(io);

		const auto notes{io.readInputFile<Pattern>()};
		validate(notes);
		io.printSolution(accumulatePatterns(notes, summarizePattern), 27202);
		io.printSolution(accumulatePatterns(notes, summarizePatternWithSmudge), 41566);
	}
}