/* This module contains the enums to year and day of the puzzle. */

module;

#include <iostream>

export module AdventDate;

//import std; // TODO: import if IntelliSense works with it



// * * * * * * * * * * * * * * * * * * * * * * * * *

export namespace AOC {
	// Day of the puzzle
	enum class Day
	{
		d01 = 1,
		d02,
		d03,
		d04,
		d05,
		d06,
		d07,
		d08,
		d09,
		d10,
		d11,
		d12,
		d13,
		d14,
		d15,
		d16,
		d17,
		d18,
		d19,
		d20,
		d21,
		d22,
		d23,
		d24,
		d25,
		dEnd		// last element can be used for loops
	};

	int operator+(Day day);				// convert Day to int via operator+
	Day& operator++(Day& day);			// increase day (for loops)



	// Year of the puzzle
	enum class Year
	{
		y2015 = 2015,
		y2016,
		y2017,
		y2018,
		y2019,
		y2020,
		y2021,
		y2022,
		y2023,
		yEnd	// last element can be used for loops
	};

	int operator+(Year year);			// convert Year to int via operator+
	Year& operator++(Year& year);		// increase year (for loops)



	struct Date
	{
		Year year{};
		Day day{};
	};

	Date& operator++(Date& date);		// increase date (for loops)

	bool operator==(Date d1, Date d2);
	bool operator<(Date d1, Date d2);

	auto& operator<<(std::ostream& out, Date d);
	void printDate(Date d);
}