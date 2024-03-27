/* This module contains tools to open the puzzle input file and read it.
   It also contains tools to print the puzzle solution.*/

module;

#include <format>
#include <fstream>
#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <Windows.h>

export module IO;

//import std; // TODO: import if IntelliSense works with it
import Errors;
import Stopwatch;
export import AdventDate;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC {
#ifdef _DEBUG
	export constexpr bool debugMode{true};
#else
//TODO:	constexpr bool debugMode{false};
	export constexpr bool debugMode{true};
#endif // _DEBUG



	void toClipboard(const std::string& s);



	export class IO
	{
	public:
		IO() = delete;
		IO(const IO&) = delete;
		IO(IO&&) = delete;
		IO(Date date);
		~IO() noexcept;

		IO& operator=(const IO&) = delete;
		IO& operator=(IO&&) = delete;

		// prints the solution for a puzzle part
		// copies the solution to the clipboard, to input it to advent of code easier
		// also compares in debugMode to an expectet value to ensure optimization didn't change the value
		template <typename T>
		void printSolution(const T& solution, const T& expected);

		void startTests() const;
		template <typename T>
		void printTest(const T& solution, const T& expected);		// prints the test for a function
		void endTests();

		std::ifstream getInputFile() const;							// get in input file, stored as *.txt in the folder "\input\"

		// Reads the input file and returns it as vector of the data set T
		// T needs operator >> (throw InvalidFileInput, if data is invalid)
		template <typename T>
		std::vector<T> readInputFile() const;



	private:
		template <typename T>
		void printTestLine(const T& solution, const T& expected, bool isTest) const;


		const Date m_date;			// Date of the puzzle
		int m_part{1};				// count how often a solution is printed
		int m_test{1};				// count how often a test is printed
		int m_failedTests{0};		// count the number of failed tests
		Stopwatch m_stopwatch{};	// for timing the code
	};



// * * * * * * * * * * * * * * * * * * * * * * * * *

	template <typename T>
	void IO::printSolution(const T& solution, const T& expected)
	{
		if (debugMode) {
			printTestLine(solution, expected, false);
		} else {
			std::println("\tPart {}: {:>16} | {:7.3f}s", m_part, solution, m_stopwatch.getTime());
		}
		++m_part;
		toClipboard(std::to_string(solution));
	}

	template <>
	void IO::printSolution(const std::string& solution, const std::string& expected)
	{
		if (debugMode) {
			printTestLine(solution, expected, false);
		} else {
			std::println("\tPart {}: {:>16} | {:7.3f}s", m_part, solution, m_stopwatch.getTime());
		}
		++m_part;
		toClipboard(solution);
	}



	template <typename T>
	void IO::printTest(const T& solution, const T& expected)
	{
		if (solution != expected) {
			++m_failedTests;
		}

		printTestLine(solution, expected, true);
		++m_test;
	}



	template <typename T>
	void IO::printTestLine(const T& solution, const T& expected, bool isTest) const
	{
		constexpr std::string_view testOutputFormat{"\t{:^16} {}{:02} | {:>16} {:>16} | {:8.3f}"};
		const std::string_view result{solution == expected ? "pass" : "***FAIL***"};

		const int index{isTest ? m_test : m_part};
		const std::string_view name{isTest ? "Test" : "Part"};

		std::println(testOutputFormat, result, name, index, solution, expected, m_stopwatch.getTime());
	}



	template <typename T>
	std::vector<T> IO::readInputFile() const
	{
		std::vector<T> result{};
		std::ifstream in{getInputFile()};

		for (std::size_t i{1}; !in.eof(); ++i) // TODO: Do not use try als flow control
		{
			T buffer{};
			try {
				in >> buffer;
				result.push_back(buffer);
			} catch (InvalidFileInput&) {
				if (in.eof()) {break;}
				throw InvalidFileLine(i);
			}
		}

		return result;
	}



	template <>
	std::vector<std::string> IO::readInputFile() const
	{
		std::vector<std::string> result{};
		std::ifstream in{getInputFile()};

		for (std::size_t i{1}; !in.eof(); ++i) {
			std::string buffer{};
			std::getline(in, buffer);
			result.push_back(buffer);
		}

		return result;
	}
}