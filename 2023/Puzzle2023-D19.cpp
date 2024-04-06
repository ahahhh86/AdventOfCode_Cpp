module;

#include <algorithm>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D19;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	struct PartStats
	{
		int x{0};
		int m{0};
		int a{0};
		int s{0};

		int getSum() const { return x + m + a + s; }
	};
	using Parts = std::vector<PartStats>;



	auto readPartStats(std::stringstream&& in)
	{
		// format example: {x=787,m=2655,a=1222,s=2876}
		using namespace std::literals;
		PartStats stats{};

		constexpr auto preX{"{x="sv.length()};
		in.ignore(preX);
		in >> stats.x;

		constexpr auto preM{",m="sv.length()};
		in.ignore(preM);
		in >> stats.m;

		constexpr auto preA{",a="sv.length()};
		in.ignore(preA);
		in >> stats.a;

		constexpr auto preS{",s="sv.length()};
		in.ignore(preS);
		in >> stats.s;

		if (in.fail()) { throw AOC::InvalidInputData{"readPartStats()"}; }

		return stats;
	}



	enum class RuleOperator
	{
		Bigger,
		Smaller,
	};

	auto charToOperator(char c)
	{
		switch (c) {
		case '>': return RuleOperator::Bigger;
		case '<': return RuleOperator::Smaller;
		default:
			throw AOC::InvalidInputData{"charToOperator()"};
		}
	}



	struct Condition
	{
		char stat{'\0'};
		RuleOperator op{RuleOperator::Bigger};
		int value{0};
	};
	constexpr int minStatValue{1};
	constexpr int maxStatValue{4000};
	//constexpr auto minStatValueLL{static_cast<long long>(minStatValue)};
	//constexpr auto maxStatValueLL{static_cast<long long>(maxStatValue)};


	auto readCondition(std::stringstream&& in)
	{
		Condition result{};

		in >> result.stat;
		switch (result.stat) {
		case 'x':
		case 'm':
		case 'a':
		case 's':
			break;
		default:
			throw AOC::InvalidInputData{"readCondition(): stat"};
		}

		char buffer{};
		in >> buffer;
		result.op = charToOperator(buffer);

		in >> result.value;
		if (in.fail()) { throw AOC::InvalidInputData{"readCondition(): invalid value"}; }
		if (result.value < minStatValue || maxStatValue < result.value) { throw AOC::InvalidInputData{"readCondition(): value out of bounds"}; }

		return result;
	}



	struct Rule
	{
		Condition condition{};
		std::string destination{""};
	};
	using RuleVector = std::vector<Rule>;



	struct Workflow
	{
		std::string name{""};
		std::string defaultDestination{""};
		RuleVector rules{};
	};
	using WorkflowVector = std::vector<Workflow>;




	auto readWorkflow(std::stringstream&& in)
	{
		// format example: nsz{x>1625:R,R}
		Workflow wf{};
		std::getline(in, wf.name, '{');

		while (true) {
			Rule rule{};

			std::string buffer{};
			std::getline(in, buffer, ':');
			if (buffer.ends_with("}")) {
				buffer.pop_back();
				wf.defaultDestination = buffer;
				break;
			} else {
				rule.condition = readCondition(std::stringstream{buffer});
				std::getline(in, rule.destination, ',');
				wf.rules.push_back(rule);
			}

		}

		return wf;
	}



	struct FileData
	{
		Parts parts{};
		WorkflowVector workflow{};
	};



	auto readFile(std::vector<std::string>&& in)
	{
		FileData result{};

		auto i{in.cbegin()};
		for ( ; *i != ""; ++i) { // an empty line separates Workflow from stats
			result.workflow.push_back(readWorkflow(std::stringstream{*i}));
		}

		++i;

		for ( ; i != in.cend(); ++i) {
			result.parts.push_back(readPartStats(std::stringstream{*i}));
		}


		return result;
	}
}



namespace { // Calculations
	constexpr std::string_view accepted{"A"};
	constexpr std::string_view rejected{"R"};



	auto checkCondition(const Condition& c, const PartStats& part)
	{
		int stat{};
		switch (c.stat) {
		case 'x':
			stat = part.x;
			break;
		case 'm':
			stat = part.m;
			break;
		case 'a':
			stat = part.a;
			break;
		case 's':
			stat = part.s;
			break;
		default:
			throw AOC::InvalidInputData{"checkCondition()"};
		}

		return c.op == RuleOperator::Bigger ? stat > c.value : stat < c.value;
	}



	auto getNextRule(const std::string& name, const PartStats& part, const WorkflowVector& wfv)
	{
		const auto workflow{std::ranges::find_if(wfv, [&](const Workflow& wf) {
			return wf.name == name;
		})};

		if (workflow == wfv.cend()) { throw AOC::InvalidInputData{"getNextRule(): rule not found"}; }

		for (const Rule& r : workflow->rules) {
			if (checkCondition(r.condition, part)) {
				return r.destination;
			}
		};

		return workflow->defaultDestination;
	}



	bool isAccepted(const PartStats& part, const WorkflowVector& wfv)
	{
		std::string ruleName{"in"};
		do {
			ruleName = getNextRule(ruleName, part, wfv);
		} while (ruleName != accepted && ruleName != rejected);

		return ruleName == accepted;
	}



	auto getAcceptedParts(const FileData& d)
	{
		Parts result{};
		result.reserve(d.parts.size());

		std::ranges::for_each(d.parts, [&](const PartStats& p) {
			if (isAccepted(p, d.workflow)) { result.push_back(p); }
		});

		return result;
	}



	auto sumAcceptedScore(const Parts& parts)
	{
		return std::accumulate(parts.cbegin(), parts.cend(), 0, [&](int sum, const PartStats& p) {
			return sum + p.getSum();
		});
	}



	auto getAcceptedScore(const FileData& d)
	{
		const Parts acceptedParts{getAcceptedParts(d)};
		return sumAcceptedScore(acceptedParts);
	}



	// TODO: Part2
	// TODO: shrink input: ldx{s>3581:R,s<3393:A,x<1640:A,A} could also be ldx{s>3581:R,A}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			const auto input{readFile({
				"px{a<2006:qkq,m>2090:A,rfg}",
				"pv{a>1716:R,A}",
				"lnx{m>1548:A,A}",
				"rfg{s<537:gd,x>2440:R,A}",
				"qs{s>3448:A,lnx}",
				"qkq{x<1416:A,crn}",
				"crn{x>2662:A,R}",
				"in{s<1351:px,qqz}",
				"qqz{s>2770:qs,m<1801:hdj,R}",
				"gd{a>3333:R,R}",
				"hdj{m>838:A,pv}",
				"",
				"{x=787,m=2655,a=1222,s=2876}",
				"{x=1679,m=44,a=2067,s=496}",
				"{x=2036,m=264,a=79,s=2244}",
				"{x=2461,m=1339,a=466,s=291}",
				"{x=2127,m=1623,a=2188,s=1013}",
				})};

			io.startTests();

			io.printTest(getAcceptedScore(input), 19114);

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D19 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d19}};
		testPuzzle(io);

		const auto fileData{readFile(io.readInputFile<std::string>())};
		io.printSolution(getAcceptedScore(fileData), 397643);
	}
}