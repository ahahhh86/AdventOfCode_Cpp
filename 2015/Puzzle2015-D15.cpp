module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

module Puzzle2015:D15;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	constexpr int spoon_count{100};



	struct SIngredientStat
	{
		std::string name{""};
		int capacity{0};
		int durability{0};
		int flavor{0};
		int texture{0};
		int calories{0};
	};



	struct SIngredients
	{
		int sugar{0};
		int sprinkles{0};
		int candy{0};
		int chocolate{0};
	};



	SIngredientStat readIngredientStat(std::ifstream& file)
	{
		SIngredientStat result{};

		std::string buffer{""};
		file >> result.name;
		result.name.pop_back(); // remove colon

		// TODO: Refactor to own function
		constexpr auto capacity_width{std::string_view("capacity ").length()};
		file.ignore(capacity_width);
		file >> result.capacity;

		constexpr auto durability_width{std::string_view(", durability ").length()};
		file.ignore(durability_width);
		file >> result.durability;

		constexpr auto flavor_width{std::string_view(", flavor ").length()};
		file.ignore(flavor_width);
		file >> result.flavor;

		constexpr auto texture_width{std::string_view(", texture ").length()};
		file.ignore(texture_width);
		file >> result.texture;

		constexpr auto calories_width{std::string_view(", calories ").length()};
		file.ignore(calories_width);
		file >> result.calories;

		if (file.fail()) throw AOC::InvalidFileInput();

		return result;
	}



	class Recipes
	{
	public:
		Recipes() = delete;
		Recipes(const Recipes&) = delete;
		Recipes(Recipes&&) = delete;
		explicit Recipes(std::ifstream input);
		~Recipes() = default;


		Recipes& operator=(const Recipes&) = delete;
		Recipes& operator=(Recipes&&) = delete;


		int calculateScore(const SIngredients ingr) const;
		int calculateCalories(const SIngredients ingr) const;

	private:
		// Assuming the same properties for each input // TODO: as vector
		const SIngredientStat m_sugarProperties;
		const SIngredientStat m_sprinklesProperties;
		const SIngredientStat m_candyProperties;
		const SIngredientStat m_chocolateProperties;
	};



	Recipes::Recipes(std::ifstream input):
		m_sugarProperties{readIngredientStat(input)},
		m_sprinklesProperties{readIngredientStat(input)},
		m_candyProperties{readIngredientStat(input)},
		m_chocolateProperties{readIngredientStat(input)}
	{}



	int Recipes::calculateScore(const SIngredients ingr) const
	{
		if ((ingr.sugar + ingr.sprinkles + ingr.candy + ingr.chocolate) != spoon_count)  throw AOC::aocError("You have to use 100 spoons.");

		const int capacity_score = std::max(
			0,
			m_sugarProperties.capacity * ingr.sugar + m_sprinklesProperties.capacity * ingr.sprinkles + m_candyProperties.capacity * ingr.candy + m_chocolateProperties.capacity * ingr.chocolate);

		const int durability_score = std::max(
			0,
			m_sugarProperties.durability * ingr.sugar + m_sprinklesProperties.durability * ingr.sprinkles + m_candyProperties.durability * ingr.candy + m_chocolateProperties.durability * ingr.chocolate);

		const int flavor_score = std::max(
			0,
			m_sugarProperties.flavor * ingr.sugar + m_sprinklesProperties.flavor * ingr.sprinkles + m_candyProperties.flavor * ingr.candy + m_chocolateProperties.flavor * ingr.chocolate);

		const int texture_score = std::max(
			0,
			m_sugarProperties.texture * ingr.sugar + m_sprinklesProperties.texture * ingr.sprinkles + m_candyProperties.texture * ingr.candy + m_chocolateProperties.texture * ingr.chocolate);

		return capacity_score * durability_score * flavor_score * texture_score;
	}



	int Recipes::calculateCalories(const SIngredients ingr) const
	{
		if ((ingr.sugar + ingr.sprinkles + ingr.candy + ingr.chocolate) != spoon_count)  throw AOC::aocError("You have to use 100 spoons.");

		return m_sugarProperties.calories * ingr.sugar + m_sprinklesProperties.calories * ingr.sprinkles + m_candyProperties.calories * ingr.candy + m_chocolateProperties.calories * ingr.chocolate;
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



namespace AOC::Y2015::D15 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d15}};
		//testPuzzle(io);

		Recipes recipes{io.getInputFile()};

		constexpr int needed_calories{500};
		SIngredients ingr{};
		int score{};
		int max_score{0};
		int max_score_calories{0};

		for (ingr.sugar = 0; ingr.sugar < spoon_count; ++ingr.sugar) {
			for (ingr.sprinkles = 0; ingr.sprinkles < (spoon_count - ingr.sugar); ++ingr.sprinkles) {
				for (ingr.candy = 0; ingr.candy < (spoon_count - ingr.sugar - ingr.sprinkles); ++ingr.candy) {
					ingr.chocolate = spoon_count - ingr.sugar - ingr.sprinkles - ingr.candy;
					score = recipes.calculateScore(ingr);

					max_score = std::max(max_score, score);
					if (recipes.calculateCalories(ingr) == needed_calories) {
						max_score_calories = std::max(max_score_calories, score);
					}
				}
			}
		}

		io.printSolution(max_score, 222870);
		io.printSolution(max_score_calories, 117936);
	}
}