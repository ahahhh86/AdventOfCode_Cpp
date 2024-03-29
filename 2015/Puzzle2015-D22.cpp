module;

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
//#include <string>
//#include <string_view>
#include <vector>

module Puzzle2015:D22;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	struct BossStats
	{
		int HP{0};
		int dmg{0};
	};



	auto readBossStats(std::ifstream&& in)
	{
		BossStats result{};

		std::string buffer{};
		in >> buffer;
		if (buffer != "Hit") { throw AOC::InvalidInputData("readBossStats()"); }
		in >> buffer;
		if (buffer != "Points:") { throw AOC::InvalidInputData("readBossStats()"); }

		in >> result.HP;

		in >> buffer;
		if (buffer != "Damage:") { throw AOC::InvalidInputData("readBossStats()"); }

		in >> result.dmg;

		if (in.fail()) { throw AOC::InvalidInputData("readBossStats()"); }

		return result;
	}
}



namespace { // Constants
	constexpr int ownStartHP{50};
	constexpr int ownStartMP{500};



/*
	Magic Missile costs 53 mana. It instantly does 4 damage.
	Drain costs 73 mana. It instantly does 2 damage and heals you for 2 hit points.
	Shield costs 113 mana. It starts an effect that lasts for 6 turns. While it is
		active, your armor is increased by 7.
	Poison costs 173 mana. It starts an effect that lasts for 6 turns. At the start
		of each turn while it is active, it deals the boss 3 damage.
	Recharge costs 229 mana. It starts an effect that lasts for 5 turns. At the start
		of each turn while it is active, it gives you 101 new mana.
*/
	enum class Spell
	{
		MagicMissile,
		Drain,
		Shield,
		Poison,
		Recharge,
	};
	using SpellVector = std::vector<Spell>;

	constexpr auto operator+(Spell v)
	{
		return static_cast<std::size_t>(v);
	}



	struct SpellStats
	{
		int cost{0};
		int value{0};
		int time{0};
	};



	constexpr std::array spellStats
	{
		SpellStats{ 53,   4, 0}, // MagicMissile
		SpellStats{ 73,   2, 0}, // Drain
		SpellStats{113,   7, 6}, // Shield
		SpellStats{173,   3, 6}, // Poison
		SpellStats{229, 101, 5}, // Recharge
	};



	enum class BattleResult
	{
		Won,
		Ongoing,
		Lost,
	};
}



namespace { // Calculations
	struct OwnStats
	{
		int HP{ownStartHP};
		int MP{ownStartMP};
		int def{0};
	};



	struct ActiveSpellTimer
	{
		int shield{-1};
		int poison{-1};
		int recharge{-1};
	};



	class Battle
	{
	public:
		explicit Battle(const BossStats& boss, const OwnStats& own);

		//auto getSelf() const { return m_self; }
		//auto getBoss() const { return m_boss; }
		//auto getTimer() const { return m_activeSpellTimer; }
		auto fight(const SpellVector& usedSpells);
		auto getManaUsage() const { return m_manaUsage; }

	private:
		auto activateSpells();
		auto castSpell(Spell spell);
		auto bossAttacks();

		auto playerTurn(const Spell usedSpell);
		auto bossTurn();

		BossStats m_boss;
		OwnStats m_self;
		int m_manaUsage{0};
		ActiveSpellTimer m_activeSpellTimer{};

		friend void testBattle(AOC::IO&, const Battle&, int, const OwnStats&);
		friend void testPuzzle(AOC::IO&);
	};



	Battle::Battle(const BossStats& boss, const OwnStats& own): m_boss{boss}, m_self{own}
	{
		// do nothing
	}



	auto Battle::activateSpells()
	{
		if (m_activeSpellTimer.shield > 0) {
			m_self.def = spellStats[+Spell::Shield].value;
			--m_activeSpellTimer.shield;
		} else {
			m_self.def = 0;
		}

		if (m_activeSpellTimer.poison > 0) {
			m_boss.HP -= spellStats[+Spell::Poison].value;
			if (m_boss.HP <= 0) { return BattleResult::Won; }
			--m_activeSpellTimer.poison;
		}

		if (m_activeSpellTimer.recharge > 0) {
			m_self.MP += spellStats[+Spell::Recharge].value;
			--m_activeSpellTimer.recharge;
		}

		return BattleResult::Ongoing;
	}



	auto Battle::castSpell(Spell spell)
	{
		switch (spell) {
		case Spell::MagicMissile:
			m_boss.HP -= spellStats[+Spell::MagicMissile].value;
			if (m_boss.HP <= 0) { return BattleResult::Won; }
			break;

		case Spell::Drain:
			m_boss.HP -= spellStats[+Spell::Drain].value;
			if (m_boss.HP <= 0) {
				//throw AOC::aocError("castSpell(): MagicMissile would have been cheaper");
				return BattleResult::Won;
			}
			m_self.HP += spellStats[+Spell::Drain].value;
			break;

		case Spell::Shield:
			m_activeSpellTimer.shield = spellStats[+Spell::Shield].time;
			break;

		case Spell::Poison:
			m_activeSpellTimer.poison = spellStats[+Spell::Poison].time;
			break;

		case Spell::Recharge:
			m_activeSpellTimer.recharge = spellStats[+Spell::Recharge].time;
			break;

		default:
			throw AOC::aocError("castSpell(): unknown spell");
		}

		m_self.MP -= spellStats[+spell].cost;
		m_manaUsage += spellStats[+spell].cost;
		
		return (m_self.MP > 0) ? BattleResult::Ongoing : BattleResult::Lost;
	}



	auto Battle::bossAttacks()
	{
		m_self.HP -= std::max(1, m_boss.dmg - m_self.def);
		return (m_self.HP > 0) ? BattleResult::Ongoing : BattleResult::Lost;
	}



	auto Battle::playerTurn(const Spell usedSpell)
	{
		const BattleResult result{activateSpells()};
		if (result != BattleResult::Ongoing) {
			return result;
		} else {
			return castSpell(usedSpell);
		}
	}



	auto Battle::bossTurn()
	{
		const BattleResult result{activateSpells()};
		if (result != BattleResult::Ongoing) {
			return result;
		} else {
			return bossAttacks();
		}
	}



	auto Battle::fight(const SpellVector& usedSpells)
	{
		for (Spell s : usedSpells) {
			BattleResult result{playerTurn(s)};
			if (result != BattleResult::Ongoing) { return result; };
			result = bossTurn();
			if (result != BattleResult::Ongoing) { return result; };
		}

		return BattleResult::Ongoing;
	}



	bool isValidSpellVector(const SpellVector& v)
	{
		for (auto i{std::next(v.cbegin())}; i != v.cend(); ++i) { // Make sure 5 turns have past, when casting recharge again
			if (*i == Spell::Recharge && *std::prev(i) == Spell::Recharge) { return false; }
		}

		for (auto i{std::next(v.cbegin(), 2)}; i != v.cend(); ++i) { // Make sure 6 turns have past, when casting poison/shield again
			if (*i == Spell::Poison && (*std::prev(i) == Spell::Poison || *std::prev(i, 2) == Spell::Poison)) { return false; }
			if (*i == Spell::Shield && (*std::prev(i) == Spell::Shield || *std::prev(i, 2) == Spell::Shield)) { return false; }
		}

		return true;
	}



	// TODO: find better way than brue force with educated guess
	auto findMinMana(const BossStats& boss)
	{
		// Making a few assumptions here
		// Poison is (mana-wise) more efficient than MagicMissile, so it will be used more often than
		// My input says boss has 55 HP, other inputs are probably in the same range
		// -> probably 6 Poison and 4 MagicMissile are enough
		// these are 20 turns (boss and self), if we are careful we would need about 6
		// we probably would not need much Drain, with 50 HP and Shield being more efficient -> 3
		// these are 2147 Mana -500 Starting Mana 1647 are left, each Rechare restores 175 Mana, so we would probably use 10

		SpellVector usedSpells{
			Spell::Poison, Spell::Poison, Spell::Poison, Spell::Poison, Spell::Poison, Spell::Poison,
			Spell::MagicMissile, Spell::MagicMissile, Spell::MagicMissile, Spell::MagicMissile,
			Spell::Shield, Spell::Shield, Spell::Shield, Spell::Shield, Spell::Shield, Spell::Shield,
			Spell::Drain, Spell::Drain, Spell::Drain,
			Spell::Recharge, Spell::Recharge, Spell::Recharge, Spell::Recharge, Spell::Recharge,
			Spell::Recharge, Spell::Recharge, Spell::Recharge, Spell::Recharge, Spell::Recharge,
		};
		std::ranges::sort(usedSpells);
		auto minMana{std::numeric_limits<int>::max()};

		do {
			if (!isValidSpellVector(usedSpells)) { continue; }

			Battle battle{boss, {ownStartHP, ownStartMP}};
			const BattleResult result{battle.fight(usedSpells)};

			if (result == BattleResult::Ongoing) { throw AOC::aocError("findMinMana(): assumption about usedSpells were not correct"); }
			if (result == BattleResult::Won) {
				minMana = std::min(minMana, battle.getManaUsage());
			}
		} while (std::next_permutation(usedSpells.begin(), usedSpells.end()));

		return minMana;
	}
}



namespace { // Testing
	void testBattle(AOC::IO& io, const Battle& battle, int expectedBossHP, const OwnStats& expectedSelf)
	{
		io.printTest(battle.m_boss.HP, expectedBossHP);
		io.printTest(battle.m_self.HP, expectedSelf.HP);
		io.printTest(battle.m_self.MP, expectedSelf.MP);
	}
	


	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {

			io.startTests();

			Battle test1{{13, 8}, {10, 250}};

			////-- Player turn --
			////- Player has 10 hit points, 0 armor, 250 mana
			////- Boss has 13 hit points
			////Player casts Poison.
			//testBattle(io, test1, 13, {10, 250});
			//io.printTest(static_cast<int>(test1.playerTurn(Spell::Poison)), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test1.getManaUsage(), 173);
			//std::cout << '\n';

			////-- Boss turn --
			////- Player has 10 hit points, 0 armor, 77 mana
			////- Boss has 13 hit points
			////Poison deals 3 damage; its timer is now 5.
			////Boss attacks for 8 damage.
			//testBattle(io, test1, 13, {10, 77});
			//io.printTest(static_cast<int>(test1.bossTurn()), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test1.getManaUsage(), 173);
			//std::cout << '\n';

			////-- Player turn --
			////- Player has 2 hit points, 0 armor, 77 mana
			////- Boss has 10 hit points
			////Poison deals 3 damage; its timer is now 4.
			////Player casts Magic Missile, dealing 4 damage.
			//testBattle(io, test1, 10, {2, 77});
			//io.printTest(static_cast<int>(test1.playerTurn(Spell::MagicMissile)), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test1.getManaUsage(), 173 + 53);
			//std::cout << '\n';

			////-- Boss turn --
			////- Player has 2 hit points, 0 armor, 24 mana
			////- Boss has 3 hit points
			////Poison deals 3 damage. This kills the boss, and the player wins.
			//testBattle(io, test1, 3, {2, 24});
			//io.printTest(static_cast<int>(test1.bossTurn()), static_cast<int>(BattleResult::Won));
			//io.printTest(test1.getManaUsage(), 173 + 53);

			// summary of the above
			io.printTest(static_cast<int>(test1.fight({Spell::Poison, Spell::MagicMissile})), static_cast<int>(BattleResult::Won));
			testBattle(io, test1, 0, {2, 24});
			io.printTest(test1.getManaUsage(), 173 + 53);
			std::cout << "\n\n\n";



			//Now, suppose the same initial conditions, except that the boss has 14 hit points instead :
			Battle test2{{14, 8}, {10, 250}};
			////-- Player turn --
			////- Player has 10 hit points, 0 armor, 250 mana
			////- Boss has 14 hit points
			////Player casts Recharge.
			//testBattle(io, test2, 14, {10, 250});
			//io.printTest(static_cast<int>(test2.playerTurn(Spell::Recharge)), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229);
			//std::cout << '\n';

			////-- Boss turn --
			////- Player has 10 hit points, 0 armor, 21 mana
			////- Boss has 14 hit points
			////Recharge provides 101 mana; its timer is now 4.
			////Boss attacks for 8 damage!
			//testBattle(io, test2, 14, {10, 21});
			//io.printTest(static_cast<int>(test2.bossTurn()), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229);
			//std::cout << '\n';

			////-- Player turn --
			////- Player has 2 hit points, 0 armor, 122 mana
			////- Boss has 14 hit points
			////Recharge provides 101 mana; its timer is now 3.
			////Player casts Shield, increasing armor by 7.
			//testBattle(io, test2, 14, {2, 122});
			//io.printTest(static_cast<int>(test2.playerTurn(Spell::Shield)), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229 + 113);
			//std::cout << '\n';

			////-- Boss turn --
			////- Player has 2 hit points, 7 armor, 110 mana
			////- Boss has 14 hit points
			////Shield's timer is now 5.
			////Recharge provides 101 mana; its timer is now 2.
			////Boss attacks for 8 - 7 = 1 damage!
			//testBattle(io, test2, 14, {2, 110});
			//io.printTest(static_cast<int>(test2.bossTurn()), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229 + 113);
			//std::cout << '\n';

			////-- Player turn --
			////- Player has 1 hit point, 7 armor, 211 mana
			////- Boss has 14 hit points
			////Shield's timer is now 4.
			////Recharge provides 101 mana; its timer is now 1.
			////Player casts Drain, dealing 2 damage, and healing 2 hit points.
			//testBattle(io, test2, 14, {1, 211});
			//io.printTest(static_cast<int>(test2.playerTurn(Spell::Drain)), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229 + 113 + 73);
			//std::cout << '\n';

			////-- Boss turn --
			////- Player has 3 hit points, 7 armor, 239 mana
			////- Boss has 12 hit points
			////Shield's timer is now 3.
			////Recharge provides 101 mana; its timer is now 0.
			////Recharge wears off.
			////Boss attacks for 8 - 7 = 1 damage!
			//testBattle(io, test2, 12, {3, 239});
			//io.printTest(static_cast<int>(test2.bossTurn()), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229 + 113 + 73);
			//std::cout << '\n';

			////-- Player turn --
			////- Player has 2 hit points, 7 armor, 340 mana
			////- Boss has 12 hit points
			////Shield's timer is now 2.
			////Player casts Poison.
			//testBattle(io, test2, 12, {2, 340});
			//io.printTest(static_cast<int>(test2.playerTurn(Spell::Poison)), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229 + 113 + 73 + 173);
			//std::cout << '\n';

			////-- Boss turn --
			////- Player has 2 hit points, 7 armor, 167 mana
			////- Boss has 12 hit points
			////Shield's timer is now 1.
			////Poison deals 3 damage; its timer is now 5.
			////Boss attacks for 8 - 7 = 1 damage!
			//testBattle(io, test2, 12, {2, 167});
			//io.printTest(static_cast<int>(test2.bossTurn()), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229 + 113 + 73 + 173);
			//std::cout << '\n';

			////-- Player turn --
			////- Player has 1 hit point, 7 armor, 167 mana
			////- Boss has 9 hit points
			////Shield's timer is now 0.
			////Shield wears off, decreasing armor by 7.
			////Poison deals 3 damage; its timer is now 4.
			////Player casts Magic Missile, dealing 4 damage.
			//testBattle(io, test2, 9, {1, 167});
			//io.printTest(static_cast<int>(test2.playerTurn(Spell::MagicMissile)), static_cast<int>(BattleResult::Ongoing));
			//io.printTest(test2.getManaUsage(), 229 + 113 + 73 + 173 + 53);
			//std::cout << '\n';

			////-- Boss turn --
			////- Player has 1 hit point, 0 armor, 114 mana
			////- Boss has 2 hit points
			////Poison deals 3 damage. This kills the boss, and the player wins.
			//testBattle(io, test2, 2, {1, 114});
			//io.printTest(static_cast<int>(test2.bossTurn()), static_cast<int>(BattleResult::Won));
			//io.printTest(test2.getManaUsage(), 229 + 113 + 73 + 173 + 53);

			// summary of the above
			io.printTest(static_cast<int>(test2.fight({Spell::Recharge, Spell::Shield, Spell::Drain, Spell::Poison, Spell::MagicMissile})), static_cast<int>(BattleResult::Won));
			testBattle(io, test2, -1, {1, 114});
			io.printTest(test2.getManaUsage(), 229 + 113 + 73 + 173 + 53);

			io.endTests();
		}
	}
}



namespace AOC::Y2015::D22 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2015, Day::d22}};
		testPuzzle(io);

		const auto boss{readBossStats(io.getInputFile())};

		//io.printSolution(findMinMana(boss), 0);
	}
}