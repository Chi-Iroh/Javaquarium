#ifdef _MSVC_LANG
#pragma warning(push, 0)
#endif

#include <vector>
#include <string_view>
#include <functional>
#include <iostream>

#ifdef _MSVC_LANG
#pragma warning(pop)
#endif

#include "Fish.hpp"

class Aquarium {
	std::vector<Fish> fishes_{};
	std::vector<Alga> algas_{ };
	static bool hasInstance_;
	std::size_t nTurns_{ 1u };

	void tryAlgaReproduction();

	void tryFishReproduction();

	void fishesEat();

	std::size_t randomFishIndex(std::size_t eaterIndex, bool usedForEating = true) const;

	void algasGrowing();

	void fishesHungry();

	void fishesGrowOlder();

	void algasGrowingOlder();

	void pause() const; // here because no reason to be accessible from outside

public:
	Aquarium();

	void addFish(std::string_view name, fishInfo::Sex sex, fishInfo::Species species);

	void addFish(std::initializer_list<Fish>&& fishes);

	void addAlga(std::size_t nAlgas = 1u);

	void turn(bool pauseBetweenEachStep = false);

	template<typename Functor>
	void turn(std::size_t nTurns, Functor whatToDoAfterEachTurn, bool pauseBetweenEachTurn = false, bool pauseBetweenEachStep = false) {
		for (; nTurns != 0u; nTurns--) {
			turn(pauseBetweenEachStep);
			std::cout << std::endl << "-----------------------------------------------Action before next turn !------------------------------------------------" << std::endl;
			whatToDoAfterEachTurn(*this);
			std::cout << newline << "------------------------------------------------------------------------------------------------------------------------" << newline;
			if (pauseBetweenEachTurn) {
				pause();
			}
		}
	}
	
	void turn(std::size_t nTurns,  bool pauseBetweenEachTurn = false, bool pauseBetweenEachStep = false);

	friend std::ostream& operator<<(std::ostream& stream, const Aquarium& aquarium);
};