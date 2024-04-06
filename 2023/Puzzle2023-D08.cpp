module;

#include <algorithm>
#include <array>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D08;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Direction
	{
		left = 0,
		right,
	};
	using Directions = std::vector<Direction>;



	std::size_t operator+(Direction value)
	{
		return static_cast<std::size_t>(value);
	}



	Direction charToDirection(char c)
	{
		switch (c) {
		case 'L':
			return Direction::left;
		case 'R':
			return Direction::right;
		default:
			throw AOC::InvalidFileInput{};
		}
	}



	auto& operator>>(std::ifstream& in, Directions& dir)
	{
		std::string buffer{};
		std::getline(in, buffer);

		dir = {};
		std::size_t index{1};

		std::ranges::for_each(buffer, [&](char c) {
			try {
				dir.push_back(charToDirection(c));
			} catch (AOC::InvalidFileInput&) {
				throw AOC::InvalidFileLine{index};
			}
			++index;
		});

		return in;
	}



	struct MapNode
	{
		std::string name{};
		std::array<std::string, 2> nextNode{};
	};
	using Network = std::vector<MapNode>;



	auto readEntry(std::ifstream& in)
	{
		constexpr std::size_t nodeNameLength{3};
		std::string buffer{};
		MapNode result{};

		in >> result.name;

		in >> buffer;
		if (buffer != "=") { throw AOC::InvalidFileInput{}; }
		in >> buffer;
		if (!buffer.starts_with("(") || !buffer.ends_with(",")) { throw AOC::InvalidFileInput{}; }

		result.nextNode[0] = buffer.substr(1, nodeNameLength);

		in >> buffer;
		if (!buffer.ends_with(")")) { throw AOC::InvalidFileInput{}; }
		result.nextNode[1] = buffer.substr(0, nodeNameLength);;

		return result;
	}



	auto& operator>>(std::ifstream& in, Network& network)
	{
		network = {};

		for (std::size_t i{1}; !in.eof(); ++i) {
			try {
				network.push_back(readEntry(in));
			} catch (AOC::InvalidFileInput&) {
				throw AOC::InvalidFileLine{i};
			}
		}

		return in;
	}



	struct PuzzleInput
	{
		Directions directions{};
		Network network{};
	};



	auto readInput(std::ifstream&& in)
	{
		PuzzleInput result{};

		in >> result.directions;
		in.ignore(1);
		in >> result.network;

		return result;
	}
}



namespace { // Calculations
	void setNextNodeName(std::string_view& nodeName, const Network& network, Direction dir)
	{
		const auto node{std::ranges::find_if(network, [&](const MapNode& node) {
			return node.name == nodeName;
		})};
		nodeName = node->nextNode[+dir];
	}



	auto calculateSteps(const PuzzleInput& input)
	{
		const auto dirSize{input.directions.size()};
		std::string_view nodeName{"AAA"};

		for (std::size_t i{0}; ; ++i) {
			setNextNodeName(nodeName, input.network, input.directions[i % dirSize]);
			if (nodeName == "ZZZ") {return i + 1;}
		}
	}



	using NodeNames = std::vector<std::string_view>;
	NodeNames getStartNodes(const Network& network)
	{
		NodeNames result{};

		std::ranges::for_each(network, [&](const MapNode& node) {
			if (node.name.ends_with("A")) {result.push_back(node.name);}
		});

		return result;
	}



	bool isAtEnd(const NodeNames& nodeNames)
	{
		for (std::string_view nName : nodeNames) {
			if (!nName.ends_with("Z")) {return false;}
		}

		return true;
	}



	auto calculateGhostSteps(const PuzzleInput& input)
	{
		const auto dirSize{input.directions.size()};
		NodeNames nodeNames{getStartNodes(input.network)};

		for (std::size_t i{0}; ; ++i) {
			const auto index{i % dirSize};
			std::ranges::for_each(nodeNames, [&](std::string_view& nName) {
				setNextNodeName(nName, input.network, input.directions[index]);
			});

			if (isAtEnd(nodeNames)) {return i + 1;}
		}
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const PuzzleInput test1{
				{Direction::right, Direction::left},
				{
					{"AAA", {{"BBB", "CCC"}}},
					{"BBB", {{"DDD", "EEE"}}},
					{"CCC", {{"ZZZ", "GGG"}}},
					{"DDD", {{"DDD", "DDD"}}},
					{"EEE", {{"EEE", "EEE"}}},
					{"GGG", {{"GGG", "GGG"}}},
					{"ZZZ", {{"ZZZ", "ZZZ"}}},
				}
			};

			const PuzzleInput test2{
				{Direction::left, Direction::left,  Direction::right},
				{
					{"AAA", {{"BBB", "BBB"}}},
					{"BBB", {{"AAA", "ZZZ"}}},
					{"ZZZ", {{"ZZZ", "ZZZ"}}},
				}
			};

			const PuzzleInput test3{
				{Direction::left,  Direction::right},
				{
					{"11A", {{"11B", "XXX"}}},
					{"11B", {{"XXX", "11Z"}}},
					{"11Z", {{"11B", "XXX"}}},
					{"22A", {{"22B", "XXX"}}},
					{"22B", {{"22C", "22C"}}},
					{"22C", {{"22Z", "22Z"}}},
					{"22Z", {{"22B", "22B"}}},
					{"XXX", {{"XXX", "XXX"}}},
				}
			};

			io.startTests();

			io.printTest(calculateSteps(test1), 2ULL);
			io.printTest(calculateSteps(test2), 6ULL);
			io.printTest(calculateGhostSteps(test3), 6ULL);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D08 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d08}};
		testPuzzle(io);

		const auto input{readInput(io.getInputFile())};
		io.printSolution(calculateSteps(input), 13771ULL); // ca. 0,3s
		//io.printSolution(calculateGhostSteps(input), 0ULL); // 2.147.483.647 too low, >2h to calculate
	}
}