module;

#include <array>
#include <fstream>
#include <limits>
#include <string_view>

module Puzzle2015:D21;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	struct BossData
	{
		int hitPoints{0};
		int damage{0};
		int armor{0};
	};



	auto readBossData(std::ifstream&& in)
	{
		BossData result{};

		constexpr auto hit_points_width{std::string_view("Hit Points: ").length()};
		in.ignore(hit_points_width);
		in >> result.hitPoints;
		constexpr auto damage_width{std::string_view("Damage: ").length()};
		in.ignore(damage_width);
		in >> result.damage;
		constexpr auto armor_width{std::string_view("Armor: ").length()};
		in.ignore(armor_width);
		in >> result.armor;

		if (in.fail()) { throw AOC::InvalidFileInput{}; }
		return result;
	}
}



namespace { // Constants
	constexpr int ownMaxHP{100};

	struct EquipmentStat
	{
		int Cost{0};
		int Damage{0};
		int Armor{0};
	};



	constexpr std::array weapons {
			// Cost, Damage, Armor
			EquipmentStat{ 8, 4, 0}, // Dagger
			EquipmentStat{10, 5, 0}, // Shortsword
			EquipmentStat{25, 6, 0}, // Warhammer
			EquipmentStat{40, 7, 0}, // Longsword
			EquipmentStat{74, 8, 0}  // Greataxe
	};
	constexpr auto weaponCount{weapons.size()};



	constexpr std::array armor {
			// Cost, Damage, Armor
			EquipmentStat{  0, 0, 0}, // None
			EquipmentStat{ 13, 0, 1}, // Leather
			EquipmentStat{ 31, 0, 2}, // Chainmail
			EquipmentStat{ 53, 0, 3}, // Splintmail
			EquipmentStat{ 75, 0, 4}, // Bandedmail
			EquipmentStat{102, 0, 5}  // Platemail
	};
	constexpr auto armorCount{armor.size()};



	constexpr std::array rings {
			// Cost, Damage, Armor
			EquipmentStat{  0, 0, 0}, // None
			EquipmentStat{ 25, 1, 0}, // Damage +1
			EquipmentStat{ 50, 2, 0}, // Damage +2
			EquipmentStat{100, 3, 0}, // Damage +3
			EquipmentStat{ 20, 0, 1}, // Defense +1
			EquipmentStat{ 40, 0, 2}, // Defense +2
			EquipmentStat{ 80, 0, 3}  // Defense +3
	};
	constexpr auto ringCount{rings.size()};
}



namespace { // Calculation
	struct EquipmentIndex
	{
		std::size_t weapon{0};
		std::size_t armor{0};
		std::size_t leftRing{0};
		std::size_t rightRing{0};
	};



	int calulateBossDamage(const EquipmentIndex& eq, int bossArmor)
	{
		const int dmg = weapons[eq.weapon].Damage + rings[eq.leftRing].Damage + rings[eq.rightRing].Damage;
		return std::max(1, dmg - bossArmor);
	}



	int calulateSelfDamage(const EquipmentIndex& eq, int bossDamage)
	{
		const int def = armor[eq.armor].Armor + rings[eq.leftRing].Armor + rings[eq.rightRing].Armor;
		return std::max(1, bossDamage - def);
	}



	int calculateCost(const EquipmentIndex& eq)
	{
		return weapons[eq.weapon].Cost + armor[eq.armor].Cost + rings[eq.leftRing].Cost + rings[eq.rightRing].Cost;
	}



	bool canIWin(const EquipmentIndex& eq, const BossData& boss)
	{
		const int boss_dmg = calulateBossDamage(eq, boss.armor);	// How much damage do I to the boss
		const int self_dmg = calulateSelfDamage(eq, boss.damage);	// How much damage does the boss to me
		const int turns{std::max(ownMaxHP / self_dmg, 1)}; 			// How many turns do I have until I die (at least one)
		const int inflicted_damage{turns * boss_dmg}; 				// How much damage can I inflict before I die

		return inflicted_damage >= boss.hitPoints;
	}



	bool sameRing(std::size_t r1, std::size_t r2)
	{
		if (r1 == 0 || r2 == 0) { return false; } // At least one hand has no ring

		return r1 == r2;
	}



	auto fightResults(const BossData& boss)
	{
		EquipmentIndex eq{};
		int minWinCost{std::numeric_limits<int>::max()};
		int maxLoseCost{0};

		for (eq.weapon = 0; eq.weapon < weaponCount; ++eq.weapon) {
			for (eq.armor = 0; eq.armor < armorCount; ++eq.armor) {
				for (eq.leftRing = 0; eq.leftRing < ringCount; ++eq.leftRing) {
					for (eq.rightRing = 0; eq.rightRing < ringCount; ++eq.rightRing) {

						if (sameRing(eq.leftRing, eq.rightRing)) { continue; }

						const int cost = calculateCost(eq);
						if (canIWin(eq, boss)) {
							minWinCost = std::min(minWinCost, cost);
						} else {
							maxLoseCost = std::max(maxLoseCost, cost);
						}

					}
				}
			}
		}

		return std::make_pair(minWinCost, maxLoseCost);
	}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			io.startTests();
	// TODO:

			//io.printTest(0, 0);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D21 {
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d21}};
		testPuzzle(io);

		const BossData boss{readBossData(io.getInputFile())};
		const auto result{fightResults(boss)};

		io.printSolution(result.first, 111);
		io.printSolution(result.second, 188);
	}
}