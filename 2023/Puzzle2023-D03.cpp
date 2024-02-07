module;

#include <algorithm>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D03;

import BasicImports;
import Point2D;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using Point = AOC::Point2D<int>;

	struct PartNumber
	{
		Point startPos{};
		int length{};
		int value{0};
	};
	using PartVector = std::vector<PartNumber>;



	using PointVector = std::vector<Point>;
	struct EngineSchematic
	{
		PointVector symbolPos{};
		PointVector gearPos{};
		PartVector enginePart{};
	};



	auto findSymbols(std::string_view str, int line)
	{
		constexpr std::string_view notSymbols{"0123456789."};
		PointVector result{};
		auto pos{str.find_first_not_of(notSymbols, 0)};

		while (pos != std::string_view::npos) {
			result.push_back({line, static_cast<int>(pos)});
			pos = str.find_first_not_of(notSymbols, pos + 1);
		}

		return result;
	}



	auto findGears(std::string_view str, int line)
	{
		constexpr std::string_view gear{"*"};
		PointVector result{};
		auto pos{str.find(gear, 0)};

		while (pos != std::string_view::npos) {
			result.push_back({line, static_cast<int>(pos)});
			pos = str.find(gear, pos + 1);
		}

		return result;
	}



	auto findEnginePart(std::string_view str, int line)
	{
		constexpr std::string_view numbers{"0123456789"};
		PartVector result{};
		auto pos{str.find_first_of(numbers, 0)};

		while (pos != std::string_view::npos) {
			auto posEnd{pos + 1};
			while (posEnd < str.size() && std::isdigit(str[posEnd])) {
				++posEnd;
			}

			auto length = posEnd - pos;
			const std::string number{str.substr(pos, length)};

			result.push_back({{line, static_cast<int>(pos)}, static_cast<int>(length), std::stoi(number)});

			pos += length;
			pos = str.find_first_of(numbers, pos + 1);
		}

		return result;
	}




	auto readSchematic(const std::vector<std::string>& input)
	{
		EngineSchematic result{};
		int line{0};

		std::ranges::for_each(input, [&](std::string_view str) {
			result.symbolPos.append_range(findSymbols(str, line));
			result.gearPos.append_range(findGears(str, line));
			result.enginePart.append_range(findEnginePart(str, line));

			++line;
		});

		return result;
	}
}



namespace { // Calculations
	bool isInRect(const Point topLeft, const Point bottomRight, const Point value)
	{
		return topLeft.x <= value.x && value.x <= bottomRight.x
			&& topLeft.y <= value.y && value.y <= bottomRight.y;
	}



	auto getPartNumber(const PartNumber& part, const PointVector& symbols)
	{
		for (const Point& pos : symbols) {
			if (isInRect({part.startPos.x - 1, part.startPos.y - 1}, {part.startPos.x + 1, part.startPos.y + part.length}, pos)) return part.value;
		}

		return 0;
	}



	auto accumulateEngineParts(const EngineSchematic& schematic)
	{
		return std::accumulate(schematic.enginePart.cbegin(), schematic.enginePart.cend(), 0, [&](int sum, const PartNumber& part) {
			return sum + getPartNumber(part, schematic.symbolPos);
		});
	}



	auto calculateGearRatio(Point gear, const PartVector& parts)
	{
		std::vector<int> nearParts{};
		const auto countNearParts{std::ranges::count_if(parts, [&](const PartNumber& i) {
			if (isInRect({i.startPos.x - 1, i.startPos.y - 1}, {i.startPos.x + 1, i.startPos.y + i.length}, gear)) {
				nearParts.push_back(i.value);
				return true;
			}
			return false;
		})};

		return countNearParts == 2 ? nearParts[0] * nearParts[1] : 0;
	}



	auto accumulateGearRatios(const EngineSchematic& schematic)
	{
		return std::accumulate(schematic.gearPos.cbegin(), schematic.gearPos.cend(), 0, [&](int sum, const Point& gear) {
			return sum + calculateGearRatio(gear, schematic.enginePart);
		});
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const std::vector<std::string> test{
				"467..114..",
				"...*......",
				"..35..633.",
				"......#...",
				"617*......",
				"..... + .58.",
				"..592.....",
				"......755.",
				"...$.*....",
				".664.598..",
			};

			const auto testSchematic{readSchematic(test)};

			io.startTests();

			io.printTest(accumulateEngineParts(testSchematic), 4361);

			io.printTest(accumulateGearRatios(testSchematic), 467835);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D03 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d03}};
		testPuzzle(io);

		const auto schematic{readSchematic(io.readInputFile<std::string>())};
		io.printSolution(accumulateEngineParts(schematic), 540131);
		io.printSolution(accumulateGearRatios(schematic), 86879020);
	}
}