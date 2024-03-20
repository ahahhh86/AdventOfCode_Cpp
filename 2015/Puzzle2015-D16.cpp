module;

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

module Puzzle2015:D16;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	struct IsAuntNum
	{
		int number{-1};
		bool found{true};
	};



	class Mfcsam
	{
	public:
		Mfcsam() = delete;
		Mfcsam(const Mfcsam&) = delete;
		Mfcsam(Mfcsam&&) = delete;
		explicit Mfcsam(std::ifstream input);
		~Mfcsam() = default;

		Mfcsam& operator=(const Mfcsam&) = delete;
		Mfcsam& operator=(Mfcsam&&) = delete;

		int getAuntPart1();
		int getAuntPart2();

	private:
		IsAuntNum isAuntPart1(const std::string& input) const;
		IsAuntNum isAuntPart2(const std::string& input) const;

		std::vector<std::string> m_input{};
		std::map<std::string, int> m_tickerTape{};
	};



	Mfcsam::Mfcsam(std::ifstream input)
	{
		m_tickerTape.insert(std::make_pair("children", 3));		// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("cats", 7));			// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("samoyeds", 2));		// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("pomeranians", 3));	// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("akitas", 0));		// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("vizslas", 0));		// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("goldfish", 5));		// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("trees", 3));		// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("cars", 2));			// @suppress("Avoid magic numbers")
		m_tickerTape.insert(std::make_pair("perfumes", 1));		// @suppress("Avoid magic numbers")

		std::string buffer{""};
		while (!input.eof()) {		// TODO: m_input as vector of class
			std::getline(input, buffer);
			m_input.push_back(buffer);
		}
	}



	int Mfcsam::getAuntPart1()
	{
		IsAuntNum result{};
		for (const auto& data : m_input) {
			result = isAuntPart1(data);
			if (result.found) {
				return result.number;
			}
		}

		return -1;
	}



	int Mfcsam::getAuntPart2()
	{
		IsAuntNum result{};
		for (const auto& data : m_input) {
			result = isAuntPart2(data);
			if (result.found) {
				return result.number;
			}
		}

		return -1;
	}



	IsAuntNum Mfcsam::isAuntPart1(const std::string& input) const
	{
		std::stringstream strstream{input};
		std::string strBuffer{};
		IsAuntNum result{};

		strstream >> strBuffer;
		if (strBuffer != "Sue") throw AOC::aocError("Invalid input. Each line should start with 'Sue '.");

		strstream >> result.number;
		if (strstream.fail()) throw AOC::aocError("Invalid input. Aunt is missing her number");

		constexpr auto separator_width{std::string_view(": ").length()}; // ': ' or ', '

		auto compareCompound = [&]() {
			int intBuffer{};

			strstream.ignore(separator_width);
			strstream >> strBuffer;
			strBuffer.pop_back(); // removes ':'
			strstream >> intBuffer;
			if (strstream.fail()) throw AOC::aocError("Invalid input. Could not read compound.");

			return m_tickerTape.at(strBuffer) == intBuffer;
		};

		while (!strstream.eof()) {
			if (!compareCompound()) {
				result.found = false;
				return result;
			}
		}

		return result;
	}



	IsAuntNum Mfcsam::isAuntPart2(const std::string& input) const
	{
		std::stringstream strstream{input};
		std::string strBuffer{""};
		int intBuffer{};
		IsAuntNum result{};

		strstream >> strBuffer;
		if (strBuffer != "Sue") throw AOC::aocError("Invalid input. Each line should start with 'Sue '.");

		strstream >> result.number;
		if (strstream.fail()) throw AOC::aocError("Invalid input. Aunt is missing her number");

		constexpr auto separator_width{std::string_view(": ").length()}; // ': ' or ', '

		auto compareCompound = [&]() {
			strstream.ignore(separator_width);
			strstream >> strBuffer;
			strBuffer.pop_back(); // removes ':'
			strstream >> intBuffer;
			if (strstream.fail()) throw AOC::aocError("Invalid input. Could not read compound.");

			if (strBuffer == "cats" || strBuffer == "trees") {
				return m_tickerTape.at(strBuffer) < intBuffer;
			} else if (strBuffer == "pomeranians" || strBuffer == "goldfish") {
				return m_tickerTape.at(strBuffer) > intBuffer;
			} else {
				return m_tickerTape.at(strBuffer) == intBuffer;
			}
		};

		while (!strstream.eof()) {
			if (!compareCompound()) {
				result.found = false;
				return result;
			}
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



namespace AOC::Y2015::D16 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d16}};
		//testPuzzle(io);

		Mfcsam mfcsam{io.getInputFile()};

		io.printSolution(mfcsam.getAuntPart1(), 103);
		io.printSolution(mfcsam.getAuntPart2(), 405);
	}
}