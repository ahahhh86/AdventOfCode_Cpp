module;

#include <format>
#include <iostream>
#include <string>

module AdventDate;

import Errors;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace AOC {
	int operator+(Day day)
	{
		return static_cast<int>(day);
	}



	Day& operator++(Day& day)
	{
		if (day == Day::dEnd) throw EnumOverflow("Day");
		day = static_cast<Day>(+day + 1);
		return day;
	}



	int operator+(Year year)
	{
		return static_cast<int>(year);
	}



	Year& operator++(Year& year)
	{
		if (year == Year::yEnd) throw EnumOverflow("Year");
		year = static_cast<Year>(+year + 1);
		return year;
	}



	Date& operator++(Date& date)
	{
		if (date.year == Year::yEnd) throw EnumOverflow("Date");

		++date.day;
		if (date.day == Day::dEnd) {
			date.day = Day::d01;
			++date.year;
		}

		return date;
	}



	bool operator==(Date d1, Date d2)
	{
		return d1.day == d2.day && d1.year == d2.year;
	}



	bool operator<(Date d1, Date d2)
	{
		if (d1.year == d2.year) {
			return d1.day < d2.day;
		} else {
			return d1.year < d2.year;
		}
	}



	auto& operator<<(std::ostream& out, Date d)
	{
		printDate(d);
		return out;
	}



	void printDate(Date d)
	{
		std::cout << std::format("Year{} | Day{:02}: ", +d.year, +d.day);
	}
}