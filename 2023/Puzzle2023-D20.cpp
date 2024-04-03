module;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
//#include <memory>
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
	struct Signal
	{
		std::string sender{""};
		Pulse pulse{Pulse::None};
		std::string destination{""};
	};
	using SignalVector = std::vector<Signal>;

	auto getNextPulses(const SignalVector& signals, ModuleVector& modules)
	{
		SignalVector result{};

		std::ranges::for_each(signals, [&](const Signal& signal) {
			auto module{std::ranges::find_if(modules, [&](const Module& m){
				return m.getName() == signal.destination;
			})};

			if (module == modules.end()) { return;/*throw AOC::InvalidInputData{"getNextPulses(): " + signal.destination};*/ }

			const Pulse newPulse{module->sendPulse(signal.pulse, signal.sender)};
			if (newPulse == Pulse::None) { return; }

			std::ranges::for_each(module->getDestinations(), [&](const std::string& dest) {
				result.push_back(Signal{module->getName(), newPulse, dest});
			});
		});

		return result;
	}


	struct CountPulses
	{
		long long low{0};
		long long high{0};
	};

	const SignalVector buttonPress{{"button", Pulse::Low, "broadcaster"}};
	auto getPulses(ModuleVector& modules)
	{
		SignalVector signals{buttonPress};

		CountPulses pulseCount{1, 0};

		do {
			signals = getNextPulses(signals, modules);
		} while (signals.size() > 0);
	}
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
				readModule(std::stringstream{"&con -> output"}),
			};
			setInputModules(test2);

			SignalVector signals{buttonPress};
			signals = getNextPulses(signals, test1);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "a"s);
			io.printTest(+signals[1].pulse, +Pulse::Low);
			io.printTest(signals[1].destination, "b"s);
			io.printTest(+signals[2].pulse, +Pulse::Low);
			io.printTest(signals[2].destination, "c"s);
			io.printTest(signals.size(), 3ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test1);
			io.printTest(+signals[0].pulse, +Pulse::High);
			io.printTest(signals[0].destination, "b"s);
			io.printTest(+signals[1].pulse, +Pulse::High);
			io.printTest(signals[1].destination, "c"s);
			io.printTest(+signals[2].pulse, +Pulse::High);
			io.printTest(signals[2].destination, "inv"s);
			io.printTest(signals.size(), 3ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test1);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "a"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test1);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "b"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test1);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "c"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test1);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "inv"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test1);
			io.printTest(+signals[0].pulse, +Pulse::High);
			io.printTest(signals[0].destination, "a"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << "\n\n\n";



			signals = getNextPulses(buttonPress, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "a"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::High);
			io.printTest(signals[0].destination, "inv"s);
			io.printTest(+signals[1].pulse, +Pulse::High);
			io.printTest(signals[1].destination, "con"s);
			io.printTest(signals.size(), 2ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "b"s);
			io.printTest(+signals[1].pulse, +Pulse::High);
			io.printTest(signals[1].destination, "output"s);
			io.printTest(signals.size(), 2ULL);
			std::cout << '\n';


			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::High);
			io.printTest(signals[0].destination, "con"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "output"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(signals.size(), 0ULL);
			std::cout << "\n\n\n";



			signals = getNextPulses(buttonPress, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "a"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "inv"s);
			io.printTest(+signals[1].pulse, +Pulse::Low);
			io.printTest(signals[1].destination, "con"s);
			io.printTest(signals.size(), 2ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::High);
			io.printTest(signals[0].destination, "b"s);
			io.printTest(+signals[1].pulse, +Pulse::High);
			io.printTest(signals[1].destination, "output"s);
			io.printTest(signals.size(), 2ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(signals.size(), 0ULL);
			std::cout << "\n\n\n";



			signals = getNextPulses(buttonPress, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "a"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::High);
			io.printTest(signals[0].destination, "inv"s);
			io.printTest(+signals[1].pulse, +Pulse::High);
			io.printTest(signals[1].destination, "con"s);
			io.printTest(signals.size(), 2ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "b"s);
			io.printTest(+signals[1].pulse, +Pulse::Low);
			io.printTest(signals[1].destination, "output"s);
			io.printTest(signals.size(), 2ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::Low);
			io.printTest(signals[0].destination, "con"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(+signals[0].pulse, +Pulse::High);
			io.printTest(signals[0].destination, "output"s);
			io.printTest(signals.size(), 1ULL);
			std::cout << '\n';

			signals = getNextPulses(signals, test2);
			io.printTest(signals.size(), 0ULL);
			std::cout << "\n\n\n";

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
		getPulses(modules);

		io.printSolution(0, 0);
	}
}