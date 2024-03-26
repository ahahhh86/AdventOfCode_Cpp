module;

#include <fstream>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D23;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using RegisterValue = unsigned int;

	enum class Instruction { Half, Triple, Inc, Jump, JumpIfEven, JumpIfOdd };
	enum class Register { a, b };



	struct Procedure
	{
		Instruction instruction{Instruction::Inc};
		Register registr{Register::a}; // register is a keyword
		int next{1};
	};
	using Program = std::vector<Procedure>;



	auto stringToInstructionValue(std::string_view str)
	{
		if (str == "hlf") {
			return Instruction::Half;
		} else if (str == "tpl") {
			return Instruction::Triple;
		} else if (str == "inc") {
			return Instruction::Inc;
		} else if (str == "jmp") {
			return Instruction::Jump;
		} else if (str == "jie") {
			return Instruction::JumpIfEven;
		} else if (str == "jio") {
			return Instruction::JumpIfOdd;
		} else {
			throw AOC::InvalidFileInput();
		}
	}



	auto strToRegister(std::string_view str) {
		if (str == "a" || str == "a,") {
			return Register::a;
		} else if (str == "b" || str == "b,") {
			return Register::b;
		} else {
			throw AOC::InvalidFileInput();
		}
	};



	auto& operator>>(std::ifstream& in, Procedure& proc)
	{
		std::string buffer{};
		in >> buffer;
		proc.instruction = stringToInstructionValue(buffer);

		switch (proc.instruction) {
		case Instruction::Jump:
			in >> proc.next;
			break;

		case Instruction::Half:
		case Instruction::Triple:
		case Instruction::Inc:
			in >> buffer;
			proc.registr = strToRegister(buffer);
			break;

		case Instruction::JumpIfEven:
		case Instruction::JumpIfOdd:
			in >> buffer;
			proc.registr = strToRegister(buffer);
			in >> proc.next;
			break;

		default:
			throw AOC::InvalidFileInput(); // Should already been handled, but switch should have a default
			break;
		}

		if (in.fail()) throw AOC::InvalidFileInput();

		return in;
	}
}



namespace { // Calculations
	struct Computer
	{
		RegisterValue a{0};
		RegisterValue b{0};
		std::size_t index{0};
	};



	Computer getNextLine(const Computer& computer, const Program& program)
	{
		Computer result{computer};
		const Procedure& proc{program[computer.index]};
		RegisterValue& registr{proc.registr == Register::a ? result.a : result.b};

		switch (proc.instruction) {
		case Instruction::Jump:
			result.index += proc.next;
			break;

		case Instruction::Half:
			registr /= 2;
			++result.index;
			break;

		case Instruction::Triple:
			registr *= 3;
			++result.index;
			break;

		case Instruction::Inc:
			++registr;
			++result.index;
			break;

		case Instruction::JumpIfEven:
			if (registr % 2 == 0) {
				result.index += proc.next;
			} else {
				++result.index;
			}
			break;

		case Instruction::JumpIfOdd:
			if (registr == 1) {
				result.index += proc.next;
			} else {
				++result.index;
			}
			break;

		default:
			throw AOC::aocError("getNextLine(): invalid instruction.");
			break;
		}

		return result;
	}



	auto computeProgam(const Program& program, RegisterValue a = 0)
	{
		Computer comp{a};

		while (comp.index < program.size()) {
			comp = getNextLine(comp, program);
		}

		return comp.b;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const Program prog{
				{Instruction::Inc, Register::b, 1},
				{Instruction::JumpIfOdd, Register::b, 2},
				{Instruction::Triple, Register::b, 1},
				{Instruction::Inc, Register::b, 1},
			};

			io.startTests();

			io.printTest(computeProgam(prog), 2U);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D23 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d23}};
		testPuzzle(io);

		const auto program{io.readInputFile<Procedure>()};

		io.printSolution(computeProgam(program), 170U);
		io.printSolution(computeProgam(program, 1), 247U);
	}
}