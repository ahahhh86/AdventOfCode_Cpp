/* This module contains error types, to find problems more easily. */

module;

#include <iostream>
#include <string>
#include <string_view>

export module Errors;

//import std; // TODO: import if IntelliSense works with it



// * * * * * * * * * * * * * * * * * * * * * * * * *

export namespace AOC {
	class aocError
	{
	public:
		aocError(std::string_view what);
		std::string_view what() const;
	private:
		std::string m_what{};
	};



	class EnumOverflow: public aocError
	{
	public:
		EnumOverflow(std::string_view enumName);
	};



	class InvalidFile: public aocError
	{
	public:
		InvalidFile(std::string_view fileName);
	};



	class InvalidFileInput: public aocError
	{
	public:
		InvalidFileInput();
	};



	class InvalidFileLine: public aocError
	{
	public:
		InvalidFileLine(std::size_t dataSetNumber);
	};



	class InvalidInputData: public aocError
	{
	public:
		InvalidInputData(std::string_view functionName);
	};
}