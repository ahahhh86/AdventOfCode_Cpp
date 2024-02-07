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
		InputWire wireIn1{""};
		InputWire wireIn2{""};
		std::string wireOut{""};
	};

	Gate readGate(std::stringstream& in)
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
			throw AOC::InvalidFileInput();
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
		if (buffer != "->") throw AOC::InvalidFileInput();
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

	WireSet readSet(std::string_view str)
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
		wireSet = readSet(buffer);

		return in;
	}
}



namespace { // Calculations
	using OptionalSignal = std::optional<Signal>;
	OptionalSignal getSignal(const InputWire& wire, const std::map<std::string, Signal>& map)
	{
		if (wire.index() == inputWireSignalIndex) {
			return std::get<Signal>(wire);
		} else {
			const std::string name{std::get<std::string>(wire)};

			if (name != "") {
				if (map.contains(name)) {
					return map.at(name);
				} else {
					return {};
				}
			}
		}

		return Signal{0};
	}



	Signal caluculateSignal(Signal a, Signal b, Gate gate)
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
		}

		return 0; // surpress warning: C4715 not all control paths return a value
	}



	void addToMap(const WireSet& wireSet, std::map<std::string, Signal>& map)
	{
		const auto wire1{getSignal(wireSet.wireIn1, map)};
		const auto wire2{getSignal(wireSet.wireIn2, map)};

		if (wire1 && wire2) {
			map.insert({wireSet.wireOut, caluculateSignal(*wire1, *wire2, wireSet.gate)});
		}
	}



	using WireSetKit = std::vector<WireSet>;
	auto calculateA(const WireSetKit& wiresSets, OptionalSignal setB = {})
	{
		std::map<std::string, Signal> wireMap{};
		if (setB) {
			wireMap.insert({"b", *setB});
		}

		do {
			std::ranges::for_each(wiresSets, [&](const WireSet& set) {
				if (!wireMap.contains(set.wireOut)) {
					addToMap(set, wireMap);
				}
			});
		} while (!wireMap.contains("a"));

		return wireMap["a"];
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const WireSetKit wireSets1{
				readSet("123 -> x"),
				readSet("456 -> y"),
				readSet("x AND y -> a"),
			};
			const WireSetKit wireSets2{
				readSet("123 -> x"),
				readSet("456 -> y"),
				readSet("x OR y -> a"),
			};
			const WireSetKit wireSets3{
				readSet("123 -> x"),
				readSet("x LSHIFT 2 -> a"),
			};
			const WireSetKit wireSets4{
				readSet("456 -> y"),
				readSet("y RSHIFT 2 -> a"),
			};
			const WireSetKit wireSets5{
				readSet("123 -> x"),
				readSet("NOT x -> a"),
			};
			const WireSetKit wireSets6{
				readSet("456 -> y"),
				readSet("NOT y -> a"),
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