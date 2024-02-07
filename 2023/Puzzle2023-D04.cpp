module;

#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

module Puzzle2023:D04;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	using NumVec = std::vector<int>;
	struct Scratchcard
	{
		NumVec winningNumbers{};
		NumVec ownedNumbers{};

		bool isValid() const;
	};



	bool Scratchcard::isValid() const
	{
		return winningNumbers.size() != 0 && ownedNumbers.size() != 0;
	}



	auto readWinningNumbers(std::stringstream& in)
	{
		NumVec result{};

		while (!in.eof()) {
			int buffer{};
			in >> buffer;
			result.push_back(buffer);

			in.ignore(1);				// skip space after the number
			if (in.peek() == '|') {
				return result;
			}
		}

		throw AOC::InvalidFileInput();
	}



	auto readOwnedNumbers(std::stringstream& in)
	{
		NumVec result{};

		while (!in.eof()) {
			int buffer{};
			in >> buffer;
			result.push_back(buffer);
		}

		return result;
	}



	auto readCard(std::stringstream&& in)
	{
		// format example: Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
		Scratchcard result{};
		std::string stringBuffer{};

		in >> stringBuffer;
		if (stringBuffer != "Card") throw AOC::InvalidFileInput();
		in >> stringBuffer; // "1:"

		result.winningNumbers = readWinningNumbers(in);
		in.ignore(std::ssize("|"));
		result.ownedNumbers = readOwnedNumbers(in);

		return result;
	}



	auto& operator>>(std::ifstream& in, Scratchcard& game)
	{
		std::string readLine{};
		std::getline(in, readLine);
		game = readCard(std::stringstream{readLine});
		if (!game.isValid()) throw AOC::InvalidFileInput();

		return in;
	}
}



namespace { // Calculations
	auto countWins(const Scratchcard& card)
	{
		return static_cast<int>(std::ranges::count_if(card.ownedNumbers, [&](int owned) {
			return std::ranges::contains(card.winningNumbers, owned);
		}));
	}



	auto calculatePoints(const Scratchcard& card)
	{
		const auto winCount{countWins(card)};
		return winCount > 2 ? static_cast<int>(1 << (winCount-1)) : winCount; // 1 << (winCount-1) = 2^(winCount-1)
	}



	using CardVector = std::vector<Scratchcard>;
	auto accumulatePoints(const CardVector& cards)
	{
		return std::accumulate(cards.cbegin(), cards.cend(), 0, [&](int sum, const Scratchcard& card){
			return sum + calculatePoints(card);
		});
	}



	auto accumulateCards(const CardVector& cards)
	{
		NumVec cardCount(cards.size());
		std::ranges::fill(cardCount, 1);			// There is at least one of each card

		std::size_t index{0};
		// if the last card add cards, you will be out of bounds
		std::for_each(cards.cbegin(), std::prev(cards.cend()), [&](const Scratchcard& card) {
			const int newCards{countWins(card)};

			if (newCards > 0)
			{
				const auto addIndexMin{index + 1};
				const auto addIndexMax{index + newCards};

				if (addIndexMin >= std::size(cardCount)) throw AOC::InvalidInputData("accumulateCards");

				for (auto j{addIndexMin}; j <= addIndexMax; ++j) {
					cardCount[j] += cardCount[index];		// the more cards you have, the more cards you win
				}
			}

			++index;
		});

		return std::accumulate(cardCount.cbegin(), cardCount.cend(), 0);
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const CardVector testCards{
				readCard(std::stringstream{"Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53"}),
				readCard(std::stringstream{"Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19"}),
				readCard(std::stringstream{"Card 3: 1 21 53 59 44 | 69 82 63 72 16 21 14  1"}),
				readCard(std::stringstream{"Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83"}),
				readCard(std::stringstream{"Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36"}),
				readCard(std::stringstream{"Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11"}),
			};

			io.startTests();

			io.printTest(calculatePoints(testCards[0]), 8);
			io.printTest(calculatePoints(testCards[1]), 2);
			io.printTest(calculatePoints(testCards[2]), 2);
			io.printTest(calculatePoints(testCards[3]), 1);
			io.printTest(calculatePoints(testCards[4]), 0);
			io.printTest(calculatePoints(testCards[5]), 0);

			io.printTest(countWins(testCards[0]), 4);
			io.printTest(countWins(testCards[1]), 2);
			io.printTest(countWins(testCards[2]), 2);
			io.printTest(countWins(testCards[3]), 1);
			io.printTest(countWins(testCards[4]), 0);
			io.printTest(countWins(testCards[5]), 0);

			io.printTest(accumulateCards(testCards), 30);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D04 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d04}};
		testPuzzle(io);

		const CardVector cards{io.readInputFile<Scratchcard>()};
		io.printSolution(accumulatePoints(cards), 21158);
		io.printSolution(accumulateCards(cards), 6050769);
	}
}