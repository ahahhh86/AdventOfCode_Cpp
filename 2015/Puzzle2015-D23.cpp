module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

module Puzzle2015:D23;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	using register_t = unsigned int;

	enum class EInstruction { hlf, tpl, inc, jmp, jie, jio };
	enum class ERegister { a, b };



	struct SInputData
	{
		EInstruction instruction{EInstruction::inc};
		ERegister registr{ERegister::a}; // register is a keyword
		int value{0};
	};
	using InputVector_t = std::vector<SInputData>;



	struct SComputerData
	{
		register_t a{0};
		register_t b{0};
		InputVector_t::size_type index{0};
	};



	SInputData readInputData(std::ifstream& input)
	{
		SInputData result{};
		std::string buffer{};

		input >> buffer;
		if (buffer == "hlf") {
			result.instruction = EInstruction::hlf;
		} else if (buffer == "tpl") {
			result.instruction = EInstruction::tpl;
		} else if (buffer == "inc") {
			result.instruction = EInstruction::inc;
		} else if (buffer == "jmp") {
			result.instruction = EInstruction::jmp;
		} else if (buffer == "jie") {
			result.instruction = EInstruction::jie;
		} else if (buffer == "jio") {
			result.instruction = EInstruction::jio;
		} else {
			throw AOC::InvalidFileInput();
		}


		const auto getRegister = [&](std::string_view str) {
			if (str == "a") {
				return ERegister::a;
			} else if (str == "b") {
				return ERegister::b;
			} else {
				throw AOC::InvalidFileInput();
			}
		};


		switch (result.instruction) {
		case EInstruction::jmp:
			input >> result.value;
			break;

		case EInstruction::hlf:
		case EInstruction::tpl:
		case EInstruction::inc:
			input >> buffer;
			result.registr = getRegister(buffer);
			break;

		case EInstruction::jie:
		case EInstruction::jio:
			input >> buffer;
			buffer.pop_back(); // remove ','
			result.registr = getRegister(buffer);
			input >> result.value;
			break;

		default:
			throw AOC::InvalidFileInput(); // Should already been handled, but switch should have a default
			break;
		}

		if(input.fail()) throw AOC::InvalidFileInput();
		return result;
	}



	// --- readInputData() ---
	InputVector_t readInputVector(std::ifstream input)
	{
		InputVector_t result{};

		while (!input.eof()) {
			result.push_back(readInputData(input));
		}

		return result;
	}



	// --- getNextLine() ---
	SComputerData getNextLine(const SComputerData& data, const InputVector_t& vec)
	{
		SComputerData result{data};
		const SInputData& input{vec[data.index]};
		register_t& reg{input.registr == ERegister::a ? result.a : result.b};

		switch (input.instruction) {
		case EInstruction::jmp:
			result.index += input.value;
			break;

		case EInstruction::hlf:
			reg /= 2;
			++result.index;
			break;

		case EInstruction::tpl:
			reg *= 3; // @suppress("Avoid magic numbers")
			++result.index;
			break;

		case EInstruction::inc:
			++reg;
			++result.index;
			break;

		case EInstruction::jie:
			if (reg % 2 == 0) {
				result.index += input.value;
			} else {
				++result.index;
			}
			break;

		case EInstruction::jio:
			if (reg == 1) {
				result.index += input.value;
			} else {
				++result.index;
			}
			break;

		default:
			throw AOC::InvalidFileInput(); // Should already been handled, but switch should have a default
			break;
		}

		return result;
	}
}



namespace { // Testing
	// TODO:
	//void testPuzzle(AOC::IO& io)
	//{
	//	if (AOC::debugMode) {
	//		const StatVector stats{
	//			{14, 10, 127},
	//			{16, 11, 162},
	//		};

	//		io.startTests();

	//		const auto raceResult{getRaceResults(stats, 1000)};
	//		io.printTest(raceResult.first, 1120);
	//		io.printTest(raceResult.second, 689);

	//		io.endTests();
	//	}
	//}
}



namespace AOC::Y2015::D23 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d23}};
		//testPuzzle(io);
		InputVector_t inputData{readInputVector(io.getInputFile())};

		SComputerData values{};
		const auto loop_end{inputData.size()};

		while (values.index < loop_end) {
			values = getNextLine(values, inputData);
		}

		io.printSolution(values.b, 170U);

		values = {1, 0, 0};
		while (values.index < loop_end) {
			values = getNextLine(values, inputData);
		}
		io.printSolution(values.b, 247U);
	}
}