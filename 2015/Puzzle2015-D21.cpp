module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

module Puzzle2015:D21;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {
	constexpr int own_max_hp{100};

	constexpr int stat_count{3};
	constexpr int stat_cost_slot{0};
	constexpr int stat_damage_slot{1};
	constexpr int stat_armor_slot{2};

	constexpr int weapon_count{5};
	constexpr int weapons_stats[weapon_count][stat_count] = {
			// Cost, Damage, Armor
			{ 8, 4, 0}, // Dagger
			{10, 5, 0}, // Shortsword
			{25, 6, 0}, // Warhammer
			{40, 7, 0}, // Longsword
			{74, 8, 0}  // Greataxe
	};

	constexpr int armor_count{6};
	constexpr int armor_stats[armor_count][stat_count] = {
			// Cost, Damage, Armor
			{  0, 0, 0}, // None
			{ 13, 0, 1}, // Leather
			{ 31, 0, 2}, // Chainmail
			{ 53, 0, 3}, // Splintmail
			{ 75, 0, 4}, // Bandedmail
			{102, 0, 5}  // Platemail
	};

	constexpr int ring_count{8};
	constexpr int ring_stats[ring_count][stat_count] = {
			// Cost, Damage, Armor
			{  0, 0, 0}, // None left hand
			{  0, 0, 0}, // None right hand
			{ 25, 1, 0}, // Damage +1
			{ 50, 2, 0}, // Damage +2
			{100, 3, 0}, // Damage +3
			{ 20, 0, 1}, // Defense +1
			{ 40, 0, 2}, // Defense +2
			{ 80, 0, 3}  // Defense +3
	};



	struct Equipment
	{
		int weapon{0};
		int armor{0};
		int leftRing{0};
		int rightRing{0};
	};



	struct SBossData
	{
		int hitPoints{0};
		int damage{0};
		int armor{0};
	};



	SBossData readBossData(std::ifstream input)
	{
		SBossData result{};

		constexpr auto hit_points_width{std::string_view("Hit Points: ").length()};
		input.ignore(hit_points_width);
		input >> result.hitPoints;
		constexpr auto damage_width{std::string_view("Damage: ").length()};
		input.ignore(damage_width);
		input >> result.damage;
		constexpr auto armor_width{std::string_view("Armor: ").length()};
		input.ignore(armor_width);
		input >> result.armor;

		if (input.fail()) throw AOC::InvalidFileInput();
		return result;
	}



	// --- calulateBossDamage() ---
	int calulateBossDamage(const Equipment& eq, int bossArmor)
	{
		constexpr int slot{stat_damage_slot};
		const int dmg = weapons_stats[eq.weapon][slot] + ring_stats[eq.leftRing][slot] + ring_stats[eq.rightRing][slot];
		return std::max(1, dmg - bossArmor);
	}



	// --- calulateSelfDamage() ---
	int calulateSelfDamage(const Equipment& eq, int bossDamage)
	{
		constexpr int slot{stat_armor_slot};
		const int def = armor_stats[eq.armor][slot] + ring_stats[eq.leftRing][slot] + ring_stats[eq.rightRing][slot];
		return std::max(1, bossDamage - def);
	}



	// --- _calculateCost() ---
	int calculateCost(const Equipment& eq)
	{
		constexpr int slot{stat_cost_slot};
		return weapons_stats[eq.weapon][slot] + armor_stats[eq.armor][slot] + ring_stats[eq.leftRing][slot] + ring_stats[eq.rightRing][slot];
	}



	// --- _canWin() ---
	bool canWin(const Equipment& eq, const SBossData& boss)
	{
		const int boss_dmg = calulateBossDamage(eq, boss.armor);	// How much damage do I to the boss
		const int self_dmg = calulateSelfDamage(eq, boss.damage);	// How much damage does the boss to me
		const int turns{std::max(own_max_hp / self_dmg, 1)}; 		// How many turns do I have until I die (at least one)
		const int inflicted_damage{turns * boss_dmg}; 				// How much damage can I inflict before I die

		return inflicted_damage >= boss.hitPoints;
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



namespace AOC::Y2015::D21 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d21}};
		//testPuzzle(io);

		const SBossData boss_status{readBossData(io.getInputFile())};

		Equipment eq{};
		int minWinCost{std::numeric_limits<int>::max()};
		int maxLoseCost{0};
		int cost{};

		for (eq.weapon = 0; eq.weapon < weapon_count; ++eq.weapon) {
			for (eq.armor = 0; eq.armor < armor_count; ++eq.armor) {
				for (eq.leftRing = 0; eq.leftRing < ring_count; ++eq.leftRing) {
					for (eq.rightRing = 1; eq.rightRing < ring_count; ++eq.rightRing) { // 0 and 1 are empty
						if ((eq.leftRing != eq.rightRing)) {
							cost = calculateCost(eq);
							if (canWin(eq, boss_status)) {
								minWinCost = std::min(minWinCost, cost);
							} else {
								maxLoseCost = std::max(maxLoseCost, cost);
							}
						}
					}
				}
			}
		}

		io.printSolution(minWinCost, 111);
		io.printSolution(maxLoseCost, 188);
	}
}