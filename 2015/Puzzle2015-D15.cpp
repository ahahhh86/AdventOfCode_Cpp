module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2015:D15;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	struct IngredientStat
	{
		//std::string name{""};
		int capacity{0};
		int durability{0};
		int flavor{0};
		int texture{0};
		int calories{0};
	};

	using StatVector = std::vector<IngredientStat>;



	auto& operator>>(std::ifstream& in, IngredientStat& stats)
	{
		// format example: Sugar: capacity 3, durability 0, flavor 0, texture -3, calories 2
		std::string buffer{""};
		in >> buffer; // ignore name

		constexpr auto capacityWidth{std::string_view("capacity ").length()};
		in.ignore(capacityWidth);
		in >> stats.capacity;

		constexpr auto durabilityWidth{std::string_view(", durability ").length()};
		in.ignore(durabilityWidth);
		in >> stats.durability;

		constexpr auto flavorWidth{std::string_view(", flavor ").length()};
		in.ignore(flavorWidth);
		in >> stats.flavor;

		constexpr auto textureWidth{std::string_view(", texture ").length()};
		in.ignore(textureWidth);
		in >> stats.texture;

		constexpr auto caloriesWidth{std::string_view(", calories ").length()};
		in.ignore(caloriesWidth);
		in >> stats.calories;

		if (in.fail()) { throw AOC::InvalidFileInput{}; }

		return in;
	}
}



namespace { // Calculations
	constexpr int spoonCount{100};

	using Ingredients = std::vector<int>;



	bool areIngredientsValid(const Ingredients& ingr)
	{
		return std::accumulate(ingr.cbegin(), ingr.cend(), 0) == spoonCount;
	}



	int calculateScore(const Ingredients& ingr, const StatVector& stats)
	{
#ifndef NDEBUG
		if (!areIngredientsValid(ingr)) { throw AOC::InvalidInputData{"calculateScore(): !areIngredientsValid(ingr)"}; }
		if (ingr.size() != stats.size()) { throw AOC::InvalidInputData{"calculateScore(): ingr.size() != stats.size()"}; }
#endif // NDEBUG

		int capacityScore{0};
		for (std::size_t i{0}; i < ingr.size(); ++i) {
			capacityScore += ingr[i] * stats[i].capacity;
		}
		if (capacityScore < 1) { return 0; }

		int durabilityScore{0};
		for (std::size_t i{0}; i < ingr.size(); ++i) {
			durabilityScore += ingr[i] * stats[i].durability;
		}
		if (durabilityScore < 1) { return 0; }

		int flavorScore{0};
		for (std::size_t i{0}; i < ingr.size(); ++i) {
			flavorScore += ingr[i] * stats[i].flavor;
		}
		if (flavorScore < 1) { return 0; }

		int textureScore{0};
		for (std::size_t i{0}; i < ingr.size(); ++i) {
			textureScore += ingr[i] * stats[i].texture;
		}
		if (textureScore < 1) { return 0; }

		return capacityScore * durabilityScore * flavorScore * textureScore;
	}



	int calculateCalories(const Ingredients& ingr, const StatVector& stats)
	{
#ifndef NDEBUG
		if (!areIngredientsValid(ingr)) { throw AOC::InvalidInputData{"calculateScore(): !areIngredientsValid(ingr)"}; }
		if (ingr.size() != stats.size()) { throw AOC::InvalidInputData{"calculateScore(): ingr.size() != stats.size()"}; }
#endif // NDEBUG

		int calories{0};
		for (std::size_t i{0}; i < ingr.size(); ++i) {
			calories += ingr[i] * stats[i].calories;
		}

		return calories;
	}



	auto getHighestScore(const StatVector& stats)
	{
		constexpr int neededCalories{500};

		int maxScore{0};
		int maxScoreCalories{0};

		// TODO; works only for four ingredients
		for (int i{0}; i < spoonCount; ++i) {
			for (int j{0}; j < (spoonCount - i); ++j) {
				for (int k = 0; k < (spoonCount - i - j); ++k) {
					const int l = spoonCount - i - j - k;
					const std::vector<int> ingr{i, j, k, l};
					const int score = calculateScore(ingr, stats);

					maxScore = std::max(maxScore, score);
					if (calculateCalories(ingr, stats) == neededCalories) {
						maxScoreCalories = std::max(maxScoreCalories, score);
					}
				}
			}
		}

		return std::make_pair(maxScore, maxScoreCalories);
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const std::vector<IngredientStat> stats {
				IngredientStat{-1, -2, 6, 3, 8},	// Butterscotch
				IngredientStat{2, 3, -2, -1, 3},		// Cinnamon
			};

			io.startTests();

			io.printTest(calculateScore({44, 56}, stats), 62842880);
			io.printTest(calculateCalories({40, 60}, stats), 500);
			io.printTest(calculateScore({40, 60}, stats), 57600000);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D15 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d15}};
		testPuzzle(io);

		const auto stats{io.readInputFile<IngredientStat>()};
		const auto result{getHighestScore(stats)};

		io.printSolution(result.first, 222870);
		io.printSolution(result.second, 117936);
	}
}