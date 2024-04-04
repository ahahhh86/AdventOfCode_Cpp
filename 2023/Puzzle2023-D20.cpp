module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

module Puzzle2023:D20;

import BasicImports;



// * * * * * * * * * * * * * * * * * * * * * * * * *

namespace { // Input
	enum class Pulse
	{
		Low,
		High,
		None,
	};

	auto operator+(Pulse p)
	{
		return static_cast<int>(p);
	}



	enum class ModulType
	{
		Brodcast,
		FlipFlop,
		Conjunction,
		TurnOn,
	};



	using Destinations = std::vector<std::string>;



	class Module;
	//using ModulePtr = std::unique_ptr<Module>;
	using ModuleVector = std::vector<Module>;

	// TODO: use Polymorphism
	class Module
	{
	public:
		//Module(): m_name{""}, m_destinations{} {}
		explicit Module(std::string_view name, const Destinations& destinations, ModulType type);

		///*virtual*/ Module& operator=(const Module& m) { m_name = m.m_name; m_destinations = m.m_destinations; return *this; }

		Destinations getDestinations() const { return m_destinations; }
		std::string getName() const { return m_name; }
		/*virtual*/ Pulse sendPulse(Pulse pulse, const std::string& sender);
		const bool& isOn() const { return m_On; }

		friend void setInputModules(ModuleVector& modules);
	private:
		Pulse getConjunctionOutput(Pulse pulse, const std::string& sender);

		std::string m_name;
		Destinations m_destinations;
		ModulType m_type;
		bool m_On{false};
		Pulse m_lastPulse{Pulse::Low};
		std::map<std::string, Pulse> m_input{};
	};

	Module::Module(std::string_view name, const Destinations& destinations, ModulType type):
		m_name{name},
		m_destinations{destinations},
		m_type{type}
	{
		// do nothing
	}



	Pulse Module::getConjunctionOutput(Pulse pulse, const std::string& sender)
	{
		if (m_input.size() < 1) { throw AOC::InvalidInputData{"getConjunctionOutput(): ill formed m_input"}; }
		if (m_input.size() == 1) { return pulse == Pulse::Low ? Pulse::High : Pulse::Low; }

		if (!m_input.contains(sender)) { throw AOC::InvalidInputData{"getConjunctionOutput(): can not find sender"}; }
		m_input[sender] = pulse;

		for (const auto& i : m_input) {
			if (i.second == Pulse::Low) { return Pulse::High; }
		}
		return Pulse::Low;
	}



	Pulse Module::sendPulse(Pulse pulse, const std::string& sender)
	{
		switch (m_type) {
		case ModulType::Brodcast:
			return pulse;
		case ModulType::FlipFlop:
			if (pulse != Pulse::Low) { return {Pulse::None}; }

			if (m_On) {
				m_On = false;
				return Pulse::Low;
			} else {
				m_On = true;
				return Pulse::High;
			}
		case ModulType::Conjunction:
			return getConjunctionOutput(pulse, sender);
		case ModulType::TurnOn:
			if (pulse == Pulse::Low) { m_On = true; }
			return Pulse::None;
		default:
			throw AOC::aocError{"Module::sendPulse()"};
		}
	}



	void setInputModules(ModuleVector& modules)
	{
		std::ranges::for_each(modules, [&](Module& module1) {
			if (module1.m_type != ModulType::Conjunction) { return; }

			std::ranges::for_each(modules, [&](Module& module2) {
					for (const auto& dest : module2.getDestinations()) {
						if (dest == module1.getName()) {
							module1.m_input.insert(std::make_pair(module2.getName(), Pulse::Low));
							break;
						}
					}
			});
		});
	}



	//class FlipFlopModule: public Module
	//{
	//public:
	//	explicit FlipFlopModule(std::string_view name, const Destinations& destinations): Module(name, destinations) {}

	//	virtual FlipFlopModule& operator=(const FlipFlopModule& m) { Module::operator=(m); m_On = m.m_On; return *this;}

	//	virtual Pulse sendPulse(Pulse pulse);

	//private:
	//	bool m_On{false};
	//};

	//Pulse FlipFlopModule::sendPulse(Pulse pulse)
	//{
	//	std::cout << "FlipFlopModule\n";
	//	if (pulse != Pulse::Low) { return {Pulse::None}; }

	//	if (m_On) {
	//		m_On = false;
	//		return Pulse::Low;
	//	} else {
	//		m_On = true;
	//		return Pulse::High;
	//	}
	//}



	//class ConjunctionModule: public Module
	//{
	//public:
	//	explicit ConjunctionModule(std::string_view name, const Destinations& destinations): Module(name, destinations) {}

	//	virtual ConjunctionModule& operator=(const ConjunctionModule& m) { Module::operator=(m); m_lastPulse = m.m_lastPulse; return *this;}

	//	virtual Pulse sendPulse(Pulse pulse);

	//private:
	//	Pulse m_lastPulse{Pulse::Low};
	//};

	//Pulse ConjunctionModule::sendPulse(Pulse pulse)
	//{
	//	std::cout << "ConjunctionModule\n";
	//	if (pulse == Pulse::None) { return Pulse::None; }

	//	return pulse == Pulse::Low ? Pulse::High : Pulse::Low;
	//}



	Module readModule(std::stringstream&& in)
	{
		std::string name{};
		in >> name;

		std::string buffer{};
		in >> buffer;
		if (buffer != "->") { throw AOC::InvalidFileInput{}; }

		Destinations dest{};
		while (!in.eof()) {
			in >> buffer;
			if (buffer.ends_with(',')) {
				buffer.pop_back();
				dest.push_back(buffer);
			} else {
				dest.push_back(buffer);
				break;
			}
		}

		if (name.starts_with('b')) { return Module{name, dest, ModulType::Brodcast}; }

		const char moduleID{name[0]};
		name.erase(0, 1);

		switch (moduleID) {
		case '%': return Module{name, dest, ModulType::FlipFlop};
		case '&': return Module{name, dest, ModulType::Conjunction};
		default:
			throw AOC::InvalidFileInput{};
		}
	}



	auto readModules(std::vector<std::string>&& in)
	{
		ModuleVector result{};

		std::ranges::for_each(in, [&](const std::string& str){
			const auto buffer{readModule(std::stringstream{str})};
			result.push_back(buffer);
		});

		setInputModules(result);
		return result;
	}
}



namespace { // Calculations
	struct PulseCount
	{
		int high{0};
		int low{0};

		long long multiply() const
		{
			return static_cast<long long>(high) * static_cast<long long>(low);
		}
	};



	struct Signal
	{
		std::string sender{""};
		Pulse pulse{Pulse::None};
		std::string destination{""};
	};
	using SignalVector = std::vector<Signal>;
	const SignalVector buttonPress{{"button", Pulse::Low, "broadcaster"}};



	class Machine
	{
	public:
		explicit Machine(const ModuleVector& modules): m_modules{modules} {/*do nothing*/}

		auto pressButton(int count);
		auto turnOn();
	private:
		void calculateNextPulses();
		void incPulseCount();
		bool findRx() const;

		ModuleVector m_modules;
		SignalVector m_signals{buttonPress};
		PulseCount m_pulseCount{0, 0};

		friend void testPuzzle(AOC::IO&);
	};



	auto Machine::pressButton(int count)
	{
		for (int i{0}; i < count; ++i) {
			do {
				// count before calculating, because we include the initial pulse
				// also the last pulse has size 0, so there is nothing to count
				incPulseCount();
				calculateNextPulses();
			} while (m_signals.size() > 0);

			m_signals = buttonPress;
		}

		return m_pulseCount.multiply();
	}



	auto Machine::turnOn()
	{
		m_modules.push_back(Module{"rx", {}, ModulType::TurnOn});
		const bool& foundRX{std::prev(m_modules.cend())->isOn()};

		for (int i{1}; i < 100000000; ++i) { // prevent infinite loop
			do {
				calculateNextPulses();
			} while (m_signals.size() > 0);

			if (foundRX) { return i; }
			m_signals = buttonPress;
		}

		throw AOC::aocError{"Machine::turnOn()"};
	}



	void Machine::calculateNextPulses()
	{
		SignalVector result{};

		std::ranges::for_each(m_signals, [&](const Signal& signal) {
			auto module{std::ranges::find_if(m_modules, [&](const Module& m) {
				return m.getName() == signal.destination;
			})};

			if (module == m_modules.end()) { return; }

			const Pulse newPulse{module->sendPulse(signal.pulse, signal.sender)};
			if (newPulse == Pulse::None) { return; }

			std::ranges::for_each(module->getDestinations(), [&](const std::string& dest) {
				result.push_back(Signal{module->getName(), newPulse, dest});
			});
		});

		m_signals = result;
	}



	void Machine::incPulseCount()
	{
		std::ranges::for_each(m_signals, [&](const Signal& s) {
			switch (s.pulse) {
			case Pulse::High:
				++m_pulseCount.high;
				break;
			case Pulse::Low:
				++m_pulseCount.low;
				break;
			default:
				break;
			}
		});
	}



	//bool Machine::findRx() const
	//{
	//	const auto result{std::ranges::find_if(m_signals, [&](const Signal& s) {
	//		return s.destination == "rx";
	//	})};

	//	if (result == m_signals.cend()) { return false; }
	//	return result->pulse == Pulse::Low;
	//}






	//auto getNextPulses(const SignalVector& signals, ModuleVector& modules)
	//{
	//	SignalVector result{};

	//	std::ranges::for_each(signals, [&](const Signal& signal) {
	//		auto module{std::ranges::find_if(modules, [&](const Module& m){
	//			return m.getName() == signal.destination;
	//		})};

	//		if (module == modules.end()) { return;/*throw AOC::InvalidInputData{"getNextPulses(): " + signal.destination};*/ }

	//		const Pulse newPulse{module->sendPulse(signal.pulse, signal.sender)};
	//		if (newPulse == Pulse::None) { return; }

	//		std::ranges::for_each(module->getDestinations(), [&](const std::string& dest) {
	//			result.push_back(Signal{module->getName(), newPulse, dest});
	//		});
	//	});

	//	return result;
	//}

	//auto getPulses(ModuleVector& modules)
	//{
	//	SignalVector signals{buttonPress};

	//	do {
	//		signals = getNextPulses(signals, modules);
	//	} while (signals.size() > 0);
	//}
}



namespace { // Testing
	void testPuzzle(AOC::IO& io)
	{
		if (AOC::debugMode) {
			using namespace std::literals::string_literals;
			io.startTests();

			ModuleVector test1{
				readModule(std::stringstream{"broadcaster -> a, b, c"}),
				readModule(std::stringstream{"%a -> b"}),
				readModule(std::stringstream{"%b -> c"}),
				readModule(std::stringstream{"%c -> inv"}),
				readModule(std::stringstream{"&inv -> a"}),
			};
			setInputModules(test1);

			ModuleVector test2{
				readModule(std::stringstream{"broadcaster -> a"}),
				readModule(std::stringstream{"%a -> inv, con"}),
				readModule(std::stringstream{"&inv -> b"}),
				readModule(std::stringstream{"%b -> con"}),
				readModule(std::stringstream{"&con -> rx"}),
			};
			setInputModules(test2);

			Machine machine1{test1};
			machine1.incPulseCount();
			machine1.calculateNextPulses();
			io.printTest(+machine1.m_signals[0].pulse, +Pulse::Low);
			io.printTest(machine1.m_signals[0].destination, "a"s);
			io.printTest(+machine1.m_signals[1].pulse, +Pulse::Low);
			io.printTest(machine1.m_signals[1].destination, "b"s);
			io.printTest(+machine1.m_signals[2].pulse, +Pulse::Low);
			io.printTest(machine1.m_signals[2].destination, "c"s);
			io.printTest(machine1.m_signals.size(), 3ULL);
			std::cout << '\n';

			machine1.incPulseCount();
			machine1.calculateNextPulses();
			io.printTest(+machine1.m_signals[0].pulse, +Pulse::High);
			io.printTest(machine1.m_signals[0].destination, "b"s);
			io.printTest(+machine1.m_signals[1].pulse, +Pulse::High);
			io.printTest(machine1.m_signals[1].destination, "c"s);
			io.printTest(+machine1.m_signals[2].pulse, +Pulse::High);
			io.printTest(machine1.m_signals[2].destination, "inv"s);
			io.printTest(machine1.m_signals.size(), 3ULL);
			std::cout << '\n';

			machine1.incPulseCount();
			machine1.calculateNextPulses();
			io.printTest(+machine1.m_signals[0].pulse, +Pulse::Low);
			io.printTest(machine1.m_signals[0].destination, "a"s);
			io.printTest(machine1.m_signals.size(), 1ULL);
			std::cout << '\n';

			machine1.incPulseCount();
			machine1.calculateNextPulses();
			io.printTest(+machine1.m_signals[0].pulse, +Pulse::Low);
			io.printTest(machine1.m_signals[0].destination, "b"s);
			io.printTest(machine1.m_signals.size(), 1ULL);
			std::cout << '\n';

			machine1.incPulseCount();
			machine1.calculateNextPulses();
			io.printTest(+machine1.m_signals[0].pulse, +Pulse::Low);
			io.printTest(machine1.m_signals[0].destination, "c"s);
			io.printTest(machine1.m_signals.size(), 1ULL);
			std::cout << '\n';

			machine1.incPulseCount();
			machine1.calculateNextPulses();
			io.printTest(+machine1.m_signals[0].pulse, +Pulse::Low);
			io.printTest(machine1.m_signals[0].destination, "inv"s);
			io.printTest(machine1.m_signals.size(), 1ULL);
			std::cout << '\n';

			machine1.incPulseCount();
			machine1.calculateNextPulses();
			io.printTest(+machine1.m_signals[0].pulse, +Pulse::High);
			io.printTest(machine1.m_signals[0].destination, "a"s);
			io.printTest(machine1.m_signals.size(), 1ULL);
			std::cout << '\n';

			machine1.incPulseCount();
			io.printTest(+machine1.m_pulseCount.low, 8);
			io.printTest(+machine1.m_pulseCount.high, 4);
			io.printTest(+machine1.m_pulseCount.multiply(), 32LL);
			std::cout << "\n\n\n";



			Machine machine2{test2};
			io.printTest(+machine2.pressButton(1000), 11687500LL);



			Machine machine3{test2};
			io.printTest(+machine3.turnOn(), 1);


			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "a"s);
			//io.printTest(machine2.m_signals.size(), 1ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[0].destination, "inv"s);
			//io.printTest(+machine2.m_signals[1].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[1].destination, "con"s);
			//io.printTest(machine2.m_signals.size(), 2ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "b"s);
			//io.printTest(+machine2.m_signals[1].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[1].destination, "rx"s);
			//io.printTest(machine2.m_signals.size(), 2ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[0].destination, "con"s);
			//io.printTest(machine2.m_signals.size(), 1ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "rx"s);
			//io.printTest(machine2.m_signals.size(), 1ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(machine2.m_signals.size(), 0ULL);
			//std::cout << "\n\n\n";



			//machine2.m_signals = buttonPress;
			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "a"s);
			//io.printTest(machine2.m_signals.size(), 1ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "inv"s);
			//io.printTest(+machine2.m_signals[1].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[1].destination, "con"s);
			//io.printTest(machine2.m_signals.size(), 2ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[0].destination, "b"s);
			//io.printTest(+machine2.m_signals[1].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[1].destination, "rx"s);
			//io.printTest(machine2.m_signals.size(), 2ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(machine2.m_signals.size(), 0ULL);
			//std::cout << "\n\n\n";



			//machine2.m_signals = buttonPress;
			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "a"s);
			//io.printTest(machine2.m_signals.size(), 1ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[0].destination, "inv"s);
			//io.printTest(+machine2.m_signals[1].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[1].destination, "con"s);
			//io.printTest(machine2.m_signals.size(), 2ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "b"s);
			//io.printTest(+machine2.m_signals[1].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[1].destination, "rx"s);
			//io.printTest(machine2.m_signals.size(), 2ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::Low);
			//io.printTest(machine2.m_signals[0].destination, "con"s);
			//io.printTest(machine2.m_signals.size(), 1ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(+machine2.m_signals[0].pulse, +Pulse::High);
			//io.printTest(machine2.m_signals[0].destination, "rx"s);
			//io.printTest(machine2.m_signals.size(), 1ULL);
			//std::cout << '\n';

			//machine2.incPulseCount();
			//machine2.calculateNextPulses();
			//io.printTest(machine2.m_signals.size(), 0ULL);
			//std::cout << "\n\n\n";

			io.endTests();
		}
	}
}



namespace AOC::Y2023::D20 { // Solution
	void solvePuzzle()
	{
		IO io{{Year::y2023, Day::d20}};
		testPuzzle(io);

		auto modules{readModules(io.readInputFile<std::string>())};

		Machine machine1{modules};
		io.printSolution(machine1.pressButton(1000), 777666211LL);

		Machine machine2{modules};
		//io.printSolution(machine2.turnOn(), 0); // TODO: takes too long; 1.000.000 is too low
	}
}