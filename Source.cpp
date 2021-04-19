#include "Aquarium.hpp"
#undef newline

#ifdef _MSVC_LANG
#pragma warning(push, 0)
#endif

#include <iostream>
#include <limits>
#include <algorithm>

#ifdef _MSVC_LANG
#pragma warning(pop)
#endif

#ifdef _WIN32
#pragma warning(push, 0)
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX
#pragma warning(pop)
#endif

template<typename ValueType>
ValueType input(std::string_view message = "", bool endlineAfterMessage = true) {
	ValueType result{};
	while (true) {
		std::cout << message;
		if (endlineAfterMessage) {
			std::cout << std::endl;
		}
		if (std::cin >> result) {
			break;
		}
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return result;
}

template<typename ValueType>
ValueType inputWhileNotIn(std::initializer_list<ValueType> allowedValues, std::string_view message = "", bool endlineAfterMessage = true) {
	ValueType value{ input<ValueType>(message, endlineAfterMessage) };
	while (std::find(allowedValues.begin(), allowedValues.end(), value) == allowedValues.end()) { // value not in allowedValues
		value = input<ValueType>(message, endlineAfterMessage);
	}
	return value;
}

int main() {
#ifdef _WIN32
	SetConsoleOutputCP(1252);
	SetConsoleCP(1252);
#endif
	const std::size_t nTurns{ input<std::size_t>("How many turns ?") };
	const bool pauseBetweenEachTurn{ inputWhileNotIn<char>({'y', 'n'}, "Do a pause between each turn (y/n) ?") == 'y' };
	const bool pauseBetweenEachStep{ inputWhileNotIn<char>({'y', 'n'}, "Do a pause between each step (y/n) ?") == 'y' };
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // empties the buffer before first call of pause();
	std::cout << std::endl << std::endl;
	Aquarium aq{};
	aq.addAlga();
	aq.addFish(
		{
			{"Herb", fishInfo::Sex::female, fishInfo::Species::sole},
			{"Carn", fishInfo::Sex::male, fishInfo::Species::tuna}
		}
	);
	aq.turn(nTurns,
		[] (Aquarium& aquarium) {
			static unsigned long fishesID{ 2u };
			aquarium.addAlga();
			aquarium.addFish(
				{
					{"Herb" + (fishesID == 1u ? "" : std::to_string(fishesID)), fishInfo::randomSex(), fishInfo::Species::sole},
					{"Carn" + (fishesID == 1u ? "" : std::to_string(fishesID)), fishInfo::randomSex(), fishInfo::Species::tuna}
				}
			);
			fishesID++;
		}, pauseBetweenEachTurn, pauseBetweenEachStep
	);
	return 0;
}