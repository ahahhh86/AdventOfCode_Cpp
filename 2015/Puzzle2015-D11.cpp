module;

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D11;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	constexpr std::size_t passwordLength{8};

	std::string getInput(const std::vector<std::string> in)
	{
		if (in.size() != 1) throw AOC::InvalidInputData("getInput(): size");
		if (in[0].size() != passwordLength) throw AOC::InvalidInputData("getInput(): length");

		std::ranges::for_each(in[0], [](char c) {
			if (!std::islower(c)) throw AOC::InvalidInputData("getInput(): not lower");
		});

		return in[0];
	}
}



namespace { // Calculations
	void increasePassword(std::string& password, std::size_t index)
	{
		if (password.size() <= index) throw AOC::InvalidInputData("increasePassword(): size");

		switch (password[index]) {
		case 'h':
		case 'k':
		case 'n':	// i, o and l are not allowed, faster than checking for it every time
			password[index] += 2;
			break;

		case 'z':	// roll over
			password[index] = 'a';
			if (index > 0) {
				increasePassword(password, index - 1);
			}
			break;

		default:
			++password[index];
			break;
		}
	}



	bool isStraight(std::string_view password)
	{
		constexpr int straightNeeded{3};

		char buffer{password[0]};
		int straightCount{1};

		for (char c : password) {
			if (c == buffer + 1) {
				++straightCount;
				if (straightCount == straightNeeded) return true;
			} else {
				straightCount = 1;
			}

			buffer = c;
		}

		return false;
	}



	bool isDouble(std::string_view password)
	{
		constexpr int doublesNeeded{2};

		int doublesCount{0};

		for (auto i{std::next(password.cbegin())}; i != password.cend(); ++i) {
			if (*i != *std::prev(i)) { continue; }

			++doublesCount;
			if (doublesCount >= doublesNeeded) { return true; }

			if (i != std::prev(password.cend())) {++i;} // else aaa would count as two doubles
		}

		return false;
	}



	bool isValid(std::string_view password)
	{
		return isStraight(password) && isDouble(password);
	}



	// normally handles by increasePassword, but needed if invalid password input
	bool hasNo_iol(std::string_view password)
	{
		return !(password.contains('i')
				 || password.contains('o')
				 || password.contains('l'));
	}



	std::string getNextPassword(std::string password)
	{
		//if the starting password is not valid, like test "ghijklmn"
		while (!hasNo_iol(password)) {
			increasePassword(password, passwordLength - 1);
		}

		do {
			increasePassword(password, passwordLength - 1);
		} while (!isValid(password));

		return password;
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();

			using namespace std::string_literals;
			io.printTest(getNextPassword("abcdefgh"), "abcdffaa"s);
			io.printTest(getNextPassword("ghijklmn"), "ghjaabcc"s);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D11 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d11}};
		testPuzzle(io);

		auto input{getInput(io.readInputFile<std::string>())};
		const std::string part1{getNextPassword(input)};

		using namespace std::string_literals;
		io.printSolution(part1, "vzbxxyzz"s);
		io.printSolution(getNextPassword(part1), "vzcaabcc"s);
	}
}