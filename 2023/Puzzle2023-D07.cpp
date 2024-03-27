module;

#include <algorithm>
#include <array>
#include <fstream>
#include <numeric>
#include <vector>

module Puzzle2023:D07;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	constexpr int handSize{5};
	enum class Card
	{
		c2 = 2,
		c3,
		c4,
		c5,
		c6,
		c7,
		c8,
		c9,
		cT,
		cJ,
		cQ,
		cK,
		cA,
	};
	using Hand = std::array<Card, handSize>;



	enum class HandType
	{
		highCard,
		onePair,
		twoPair,
		threeOfAKind,
		fullHouse,
		fourOfAKind,
		fiveOfAKind,
	};



	struct HandAndBid
	{
		Hand hand{};
		int bid{0};
	};



	auto charToCard(char c)
	{
		switch (c) {
		case '2': return Card::c2;
		case '3': return Card::c3;
		case '4': return Card::c4;
		case '5': return Card::c5;
		case '6': return Card::c6;
		case '7': return Card::c7;
		case '8': return Card::c8;
		case '9': return Card::c9;
		case 'T': return Card::cT;
		case 'J': return Card::cJ;
		case 'Q': return Card::cQ;
		case 'K': return Card::cK;
		case 'A': return Card::cA;
		default: throw AOC::InvalidFileInput();
		}
	}



	auto& operator>>(std::ifstream& in, HandAndBid& hand)
	{
		std::ranges::for_each(hand.hand, [&](Card& card) {
			char buffer{};
			in >> buffer;
			card = charToCard(buffer);
		});

		in >> hand.bid;

		return in;
	}
}



namespace { // Calculations
	auto countCards(Card card, const Hand hand)
	{
		return static_cast<int>(std::ranges::count_if(hand, [&](Card c) {
			return card == c;
		}));
	}



	auto getHandTypeNoJoker(int cardSum)
	{
		switch (cardSum) {
		case 25: return HandType::fiveOfAKind;	// cardCount = {5, 5, 5, 5, 5}
		case 17: return HandType::fourOfAKind;	// cardCount = {4, 4, 4, 4, 1}
		case 13: return HandType::fullHouse;	// cardCount = {3, 3, 3, 2, 2}
		case 11: return HandType::threeOfAKind;	// cardCount = {3, 3, 3, 1, 1}
		case  9: return HandType::twoPair;		// cardCount = {2, 2, 2, 2, 1}
		case  7: return HandType::onePair;		// cardCount = {2, 2, 1, 1, 1}
		case  5: return HandType::highCard;		// cardCount = {1, 1, 1, 1, 1}
		default: throw AOC::InvalidInputData("getHandTypeNoJoker()");
		}
	}
}



namespace Part1 { // Calculations
	namespace {
		auto getHandType(const Hand& hand)
		{
			std::array<int, handSize> cardCount{};
			for (std::size_t i{0}; i < handSize; ++i) {
				cardCount[i] = countCards(hand[i], hand);
			}

			const auto cardSum{std::accumulate(cardCount.cbegin(), cardCount.cend(), 0)};

			return getHandTypeNoJoker(cardSum);
		}



		bool isSmallerHand(const Hand& h1, const Hand& h2)
		{
			const auto handType1{getHandType(h1)};
			const auto handType2{getHandType(h2)};

			if (handType1 == handType2) {
				for (std::size_t i{0}; i < handSize; ++i) {
					if (h1[i] != h2[i]) {return h1[i] < h2[i];}
				}

				throw AOC::InvalidInputData("Part1::isSmallerHand()");
			} else {
				return handType1 < handType2;
			}
		}



		bool isSmaller(const HandAndBid& h1, const HandAndBid& h2)
		{
			return isSmallerHand(h1.hand, h2.hand);
		}
	}
}



namespace Part2 { // Calculations
	namespace {
		auto getHandTypeOneJoker(int cardSum)
		{
			switch (cardSum) {
			case 16: return HandType::fiveOfAKind;	// cardCount = {4, 4, 4, 4, 0}
			case 10: return HandType::fourOfAKind;	// cardCount = {3, 3, 3, 1, 0}
			case  8: return HandType::fullHouse;	// cardCount = {2, 2, 2, 2, 0}
			case  6: return HandType::threeOfAKind;	// cardCount = {2, 2, 1, 1, 0}
			case  4: return HandType::onePair;		// cardCount = {1, 1, 1, 1, 0}
			default: throw AOC::InvalidInputData("Part2::getHandTypeOneJoker()");
			}
		}



		auto getHandTypeTwoJokers(int cardSum)
		{
			switch (cardSum) {
			case 9: return HandType::fiveOfAKind;	// cardCount = {3, 3, 3, 0, 0}
			case 5: return HandType::fourOfAKind;	// cardCount = {2, 2, 1, 0, 0}
			case 3: return HandType::threeOfAKind;	// cardCount = {1, 1, 1, 0, 0}
			default: throw AOC::InvalidInputData("Part2::getHandTypeTwoJokers()");
			}
		}



		auto getHandTypeThreeJokers(int cardSum)
		{
			switch (cardSum) {
			case 4: return HandType::fiveOfAKind;	// cardCount = {2, 2, 0, 0, 0}
			case 2: return HandType::fourOfAKind;	// cardCount = {1, 1, 0, 0, 0}
			default: throw AOC::InvalidInputData("Part2::getHandTypeThreeJokers()");
			}
		}



		auto getHandType(const Hand& hand)
		{
			std::array<int, handSize> cardCount{};
			int jokerCount{0};

			for (std::size_t i{0}; i < handSize; ++i) {
				if (hand[i] == Card::cJ) {
					cardCount[i] = 0;
					++jokerCount;
				} else {
					cardCount[i] = countCards(hand[i], hand);
				}
			}

			const auto cardSum{std::accumulate(cardCount.cbegin(), cardCount.cend(), 0)};

			switch (jokerCount) {
			case 0: return getHandTypeNoJoker(cardSum);
			case 1: return getHandTypeOneJoker(cardSum);
			case 2: return getHandTypeTwoJokers(cardSum);
			case 3: return getHandTypeThreeJokers(cardSum);
			case 4: // fallthrough
			case 5: return HandType::fiveOfAKind;
			default: throw AOC::InvalidInputData("Part2::getHandType()");
			}

		}



		bool isSmallerHand(const Hand& h1, const Hand& h2)
		{
			const HandType handType1{getHandType(h1)};
			const HandType handType2{getHandType(h2)};

			if (handType1 == handType2) {
				for (std::size_t i{0}; i < handSize; ++i) {
					if (h1[i] != h2[i]) {
						if (h1[i] == Card::cJ) return true;
						if (h2[i] == Card::cJ) return false;
						return h1[i] < h2[i];
					}
				}

				throw AOC::InvalidInputData("Part2::isSmallerHand()");
			} else {
				return handType1 < handType2;
			}
		}



		bool isSmaller(const HandAndBid& h1, const HandAndBid& h2)
		{
			return isSmallerHand(h1.hand, h2.hand);
		}
	}
}



namespace {
	using PuzzleInput = std::vector<HandAndBid>;

	int calculateBid(PuzzleInput cards, bool(*fcn)(const HandAndBid&, const HandAndBid&))
	{
		std::ranges::sort(cards, fcn);

		int index{0};
		return std::accumulate(cards.cbegin(), cards.cend(), 0, [&](int sum, const HandAndBid& hand) {
			++index;
			return sum + index * hand.bid;
		});
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			PuzzleInput test{
				{{{Card::c3, Card::c2, Card::cT, Card::c3, Card::cK}}, 765},
				{{{Card::cT, Card::c5, Card::c5, Card::cJ, Card::c5}}, 684},
				{{{Card::cK, Card::cK, Card::c6, Card::c7, Card::c7}}, 28},
				{{{Card::cK, Card::cT, Card::cJ, Card::cJ, Card::cT}}, 220},
				{{{Card::cQ, Card::cQ, Card::cQ, Card::cJ, Card::cA}}, 483},
			};

			io.startTests();

			io.printTest(calculateBid(test, Part1::isSmaller), 6440);
			io.printTest(calculateBid(test, Part2::isSmaller), 5905);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D07 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d07}};
		testPuzzle(io);

		const auto input{io.readInputFile<HandAndBid>()};
		io.printSolution(calculateBid(input, Part1::isSmaller), 252052080);
		io.printSolution(calculateBid(input, Part2::isSmaller), 252898370);
	}
}