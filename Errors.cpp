/* This module contains error types, to find problems more easily. */

module;

#include <format>
#include <iostream>
#include <string>
#include <string_view>

module Errors;

//import std; // TODO: import if IntelliSense works with it



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC {
	std::string_view aocError::what() const
	{
		return m_what;
	}



	aocError::aocError(std::string_view what)
	{
		m_what = what.data();
	}



	EnumOverflow::EnumOverflow(std::string_view enumName):
		aocError(std::format("Enum {} is overflowing", enumName))
	{
	}



	InvalidFile::InvalidFile(std::string_view filename):
		aocError(std::format("Could not open input file: {}", filename))
	{
	}



	InvalidFileInput::InvalidFileInput():
		aocError(std::format("InvalidFileInput"))
	{
	}



	InvalidFileLine::InvalidFileLine(std::size_t dataSetNumber):
		aocError(std::format("Read invalid input @data#{}", dataSetNumber))
	{
	}



	InvalidInputData::InvalidInputData(std::string_view functionName):
		aocError(std::format("Invalid input. This should have been checked before. @{}", functionName))
	{
	}
}