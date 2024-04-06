module;

#include <algorithm>
#include <fstream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

module Puzzle2015:D07;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Gate
	{
		gAssign,
		gNOT,
		gAND,
		gOR,
		gLShift,
		gRShift,
	};

	using Signal = uint16_t;
	using InputWire = std::variant<Signal, std::string>;
	constexpr int inputWireSignalIndex{0};
	constexpr int inputWireStringIndex{1};



	struct WireSet
	{
		Gate gate{};
		InputWire wireIn1{Signal{0}};
		InputWire wireIn2{Signal{0}};
		std::string wireOut{""};
	};



	auto readGate(std::stringstream& in)
	{
		std::string buffer{};
		in >> buffer;

		if (buffer == "AND") {
			return Gate::gAND;
		} else if (buffer == "OR") {
			return Gate::gOR;
		} else if (buffer == "LSHIFT") {
			return Gate::gLShift;
		} else if (buffer == "RSHIFT") {
			return Gate::gRShift;
		} else {
			throw AOC::InvalidFileInput{};
		}
	}

	InputWire readWireIn(std::stringstream& in)
	{
		Signal intBuffer{};
		in >> intBuffer;

		if (!in.fail()) {
			return intBuffer;
		} else {
			in.clear(std::ios_base::failbit & std::ios::badbit);
			std::string strBuffer{};
			in >> strBuffer;
			return strBuffer;
		}
	}

	auto readWireOut(std::stringstream& in)
	{
		std::string buffer{};
		in >> buffer;
		if (buffer != "->") { throw AOC::InvalidFileInput{}; };
		in >> buffer;
		return buffer;
	}



	auto readAssign(std::stringstream&& in)
	{
		WireSet result{Gate::gAssign};

		result.wireIn1 = readWireIn(in);
		result.wireOut = readWireOut(in);

		return result;
	}

	auto readNOT(std::stringstream&& in)
	{
		WireSet result{Gate::gNOT};

		in.ignore(std::ssize("NOT"));
		result.wireIn1 = readWireIn(in);
		result.wireOut = readWireOut(in);

		return result;
	}

	auto readBinary(std::stringstream&& in)
	{
		WireSet result{};

		result.wireIn1 = readWireIn(in);
		result.gate = readGate(in);
		result.wireIn2 = readWireIn(in);
		result.wireOut = readWireOut(in);

		return result;
	}



	auto readWireSet(std::string_view str)
	{
		if (str.find("NOT ") != std::string::npos) {
			return readNOT(std::stringstream{str.data()});
		} else if (str.find(" AND ") != std::string::npos
				   || str.find(" OR ") != std::string::npos
				   || str.find(" RSHIFT ") != std::string::npos
				   || str.find(" LSHIFT ") != std::string::npos) {
			return readBinary(std::stringstream{str.data()});
		} else {
			return readAssign(std::stringstream{str.data()});
		}

	}

	auto& operator>>(std::ifstream& in, WireSet& wireSet)
	{
		std::string buffer{};
		std::getline(in, buffer);
		wireSet = readWireSet(buffer);

		return in;
	}
}



namespace { // Calculations
	using OptionalSignal = std::optional<Signal>;
	using SignalMap = std::map<std::string, Signal>;

	OptionalSignal getSignal(const InputWire& wire, const SignalMap& map)
	{
		if (wire.index() == inputWireSignalIndex) {
			return std::get<Signal>(wire);
		} else {
			const std::string name{std::get<std::string>(wire)};

			if (map.contains(name)) {
				return map.at(name);
			} else {
				return {};
			}
		}
	}



	Signal calculateSignal(Signal a, Signal b, Gate gate)
	{
		switch (gate) {
		case Gate::gAssign:
			return a;
		case Gate::gNOT:
			return static_cast<Signal>(~a);			// static_cast to prevent signed/unsigned mismatch warning
		case Gate::gAND:
			return static_cast<Signal>(a & b);
		case Gate::gOR:
			return static_cast<Signal>(a | b);
		case Gate::gLShift:
			return static_cast<Signal>(a << b);
		case Gate::gRShift:
			return static_cast<Signal>(a >> b);
		default:
			throw AOC::aocError{"calculateSignal(): Not all cases of Gate have been included."};
		}
	}



	void addToMap(const WireSet& wireSet, SignalMap& map)
	{
		const auto wire1{getSignal(wireSet.wireIn1, map)};
		const auto wire2{getSignal(wireSet.wireIn2, map)};

		if (wire1 && wire2) {
			map.insert({wireSet.wireOut, calculateSignal(*wire1, *wire2, wireSet.gate)});
		}
	}



	using WireSetKit = std::vector<WireSet>;
	auto calculateA(const WireSetKit& wiresSets, OptionalSignal setB = {})
	{
		SignalMap signalMap{};
		if (setB) {
			signalMap.insert({"b", *setB});
		}

		do {
			std::ranges::for_each(wiresSets, [&](const WireSet& set) {
				if (!signalMap.contains(set.wireOut)) {
					addToMap(set, signalMap);
				}
			});
		} while (!signalMap.contains("a"));

		return signalMap["a"];
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const WireSetKit wireSets1{
				readWireSet("123 -> x"),
				readWireSet("456 -> y"),
				readWireSet("x AND y -> a"),
			};
			const WireSetKit wireSets2{
				readWireSet("123 -> x"),
				readWireSet("456 -> y"),
				readWireSet("x OR y -> a"),
			};
			const WireSetKit wireSets3{
				readWireSet("123 -> x"),
				readWireSet("x LSHIFT 2 -> a"),
			};
			const WireSetKit wireSets4{
				readWireSet("456 -> y"),
				readWireSet("y RSHIFT 2 -> a"),
			};
			const WireSetKit wireSets5{
				readWireSet("123 -> x"),
				readWireSet("NOT x -> a"),
			};
			const WireSetKit wireSets6{
				readWireSet("456 -> y"),
				readWireSet("NOT y -> a"),
			};

			io.startTests();

			io.printTest(calculateA(wireSets1), Signal{72});
			io.printTest(calculateA(wireSets2), Signal{507});
			io.printTest(calculateA(wireSets3), Signal{492});
			io.printTest(calculateA(wireSets4), Signal{114});
			io.printTest(calculateA(wireSets5), Signal{65412});
			io.printTest(calculateA(wireSets6), Signal{65079});

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D07 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d07}};
		testPuzzle(io);

		const auto wireSets{io.readInputFile<WireSet>()};
		const auto part1{calculateA(wireSets)};

		io.printSolution(part1, Signal{3176});
		io.printSolution(calculateA(wireSets, part1), Signal{14710});
	}
}