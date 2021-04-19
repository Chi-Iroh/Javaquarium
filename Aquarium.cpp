#include "Aquarium.hpp"

#ifdef _MSVC_LANG
#pragma warning(push, 0)
#endif

#include <cassert>
#include <random>
#include <chrono>

#ifdef _MSVC_LANG
#pragma warning(pop)
#endif

void Aquarium::pause() const {
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool Aquarium::hasInstance_{ false };

void Aquarium::algasGrowing() {
	for (auto& alga : algas_) {
		alga.grow();
		std::cout << std::endl << "An alga grew, recovered 1 HP and has now " << alga.HP() << " HP !";
	}
	if (!algas_.empty()) {
		std::cout << newline;
	}
}

void Aquarium::fishesHungry() {
	for (auto& fish : fishes_) {
		fish.getHungry();
		if (fish.HP() == 0u) {
			std::cout << fish.name() << " is too hungry and died !" << std::endl;
			continue;
		}
		std::cout << std::endl << fish.name() << " becomes hungrier, losts 1 HP and has now " << fish.HP() << " HP ! ";
	}
	std::erase_if(fishes_, [](Fish& fish) {return fish.HP() == 0; });
	if (!fishes_.empty()) {
		std::cout << newline;
	}
}

void Aquarium::fishesGrowOlder() {
	for (auto& fish : fishes_) {
		try {
			fish.getOlder();
			std::cout << std::endl << fish.name() << " is growing older and has now ";
			if (fish.age() == 1u) {
				std::cout << "only one year !";
			}
			else {
				std::cout << fish.age() << " years !";
			}
		}
		catch (std::runtime_error&) {
			std::cout << std::endl << fish.name() << " is 20 years and dies of old age !";
		}
	}
	std::erase_if(fishes_, [](const auto& fish) {return fish.age() >= LivingBeing::maxAge; });
	if (!fishes_.empty()) {
		std::cout << newline;
	}
}

void Aquarium::algasGrowingOlder() {
	for (auto& alga : algas_) {
		try {
			alga.getOlder();
			std::cout << std::endl << "An alga is growing older and has now ";
			if (alga.age() == 1u) {
				std::cout << "only one year !";
			}
			else {
				std::cout << alga.age() << " years !";
			}
		}
		catch (std::runtime_error&) {
			std::cout << std::endl << "An alga has 20 years and dies of old age !";
		}
	}
	std::erase_if(algas_, [](const auto& alga) {return alga.age() >= LivingBeing::maxAge; });
	if (!algas_.empty()) {
		std::cout << newline;
	}
}

void Aquarium::tryAlgaReproduction() {
	if (algas_.size() > 0u) {
		auto reproductableAlgaPos{ std::find_if(algas_.begin(), algas_.end(), [](auto& alga) {return alga.HP() >= 10u; }) };
		if (reproductableAlgaPos != algas_.cend()) {
			const auto newParentHP{ reproductableAlgaPos->HP() / 2u };
			const auto childHP{ reproductableAlgaPos->HP() - newParentHP };
			std::cout << std::endl << "An alga with " << reproductableAlgaPos->HP() << " HP has split and gave birth to a new alga ! The parent has now " << newParentHP << " HP and the child has " << childHP << " HP !";
			reproductableAlgaPos->getDamage(childHP);
			algas_.emplace_back(0u, childHP);
		}
		else {
			std::cout << std::endl << "None alga can reproduce itself (none has 10 HP or more) !";
		}
	}
	else {
		std::cout << std::endl << "None alga can reproduce itself because there isn't any alga in the aquarium !";
	}
	std::cout << newline;
}

void Aquarium::tryFishReproduction() {
	// if Fish1 and Fish2 give birth to Fish1son and a 2nd time Fish1son, the 2nd son will be called Fish1son2 etc..
	static std::map<std::string, std::size_t> nTimesThereAreTheSameParents{};

	// shuffles the fishes, because the for loop will iterate in the order : index0, index1 etc..
	std::shuffle(fishes_.begin(), fishes_.end(), std::default_random_engine());

	if (fishes_.size() > 1u) { // 1st and last fish try to reproduce
		auto& fish{ fishes_.front() };
		const auto& other{ fishes_.back() };

		if (fish.canReproduceWith(other)) {
			const std::string fishName{ fish.name() };
			const std::string otherName{ other.name() };
			const auto childSex{ (randomLivingBeingAge() % 2 == 0u) ? fishInfo::Sex::male : fishInfo::Sex::female };

			// chooses randomly the name and adds "son" or "san" if the child's male or female
			const std::string name{ ((randomLivingBeingAge() % 2 == 0u) ? fish.name() : other.name()) + ((childSex == fishInfo::Sex::female) ? "san" : "son") }; 

			if (nTimesThereAreTheSameParents.find(name) == nTimesThereAreTheSameParents.end()) {
				nTimesThereAreTheSameParents.emplace(name, 1u); // adds the name
			}
			else {
				nTimesThereAreTheSameParents.at(name)++; // increases the number of occurrences of the name
			}

			const auto nTimesName{ nTimesThereAreTheSameParents.at(name) };
			
			if (nTimesName > 1u) {
				fishes_.push_back(fish.reproduceWith(other, childSex, name + std::to_string(nTimesName)));
			}
			else {
				fishes_.push_back(fish.reproduceWith(other, childSex, name));
			}
			std::cout << std::endl << fishName << " and " << otherName << " gave birth to " << fishes_.back().name() << " !";
		}
		else {
			std::cout << std::endl << "Unfortunately, " << fish.name() << " and " << other.name() << " cannot reproduce !";
		}
	}
	else {
		std::cout << std::endl << "The aquarium doesn't contain any fish, so none fish reproduction is possible !";
	}
	std::cout << newline;
}

void Aquarium::fishesEat() {
	for (std::size_t index{ 0u }; index < fishes_.size(); index++) {
		auto& fish{ fishes_[index] };
		if (fish.needsToEat()) {
			if (fish.isCarnivorous()) {
				if (fishes_.size() > 1u) {
					try {
						const auto indexOfFishAte{ randomFishIndex(index) }; // may throw
						auto& target{ fishes_[indexOfFishAte] };
						fish.eat(target);
						if (target.HP() == 0u) {
							fishes_.erase(fishes_.begin() + static_cast<long>(indexOfFishAte));
							index--;
						}
					}
					catch (std::runtime_error&) {
						std::cout << std::endl << "Unfortunately, " << fish.name() << " cannot eat a fish, because they all belong to the same species !";
					}
				}
				else {
					std::cout << std::endl << "Unfortunately, " << fish.name() << " cannot eat another fish because he's alone !";
				}
			}
			else {
				if (!algas_.empty()) {
					fish.eat(algas_.back());
					if (algas_.back().HP() == 0u) {
						algas_.pop_back();
					}
				}
				else {
					std::cout << std::endl << "Unfortunately, " << fish.name() << " cannot eat an alga because there isn't any !";
				}
			}
		}
		else {
			std::cout << std::endl << fish.name() << " isn't hungry !";
		}
	}
	if (!fishes_.empty()) {
		std::cout << newline;
	}
}

std::size_t Aquarium::randomFishIndex(std::size_t fishIndex, bool usedForEating) const {
	static std::random_device device{};
	std::mt19937 generator{
		static_cast<std::mt19937::result_type>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
		)
	};
	// size is checked in fishEat function; not static because size may change between two calls
	std::uniform_int_distribution<std::size_t> distribution{ 0, fishes_.size() - 1u };
	if (!usedForEating) {
		std::size_t index{fishIndex};
		while (index == fishIndex) { 
			index = distribution(generator); 
		}
		return index;
	}
	auto belongToSameSpecie{ [](const Fish& first, const Fish& second) {
		return first.species() == second.species();
	} };
	{
		bool fishCanEat{ false };
		for (std::size_t index{ 0u }; const auto & fish : fishes_) {
			if (index != fishIndex && !belongToSameSpecie(fish, fishes_[fishIndex])) {
				fishCanEat = true;
				break;
			}
			index++;
		}
		if (!fishCanEat) {
			throw std::runtime_error("Fish cannot eat !"); // caught in Aquarium::fishEat
		}
	}
	std::size_t index{ fishIndex };
	while (index == fishIndex || belongToSameSpecie(fishes_[index], fishes_[fishIndex])) { // the fish cannot eat himself or a fish with the same species
		index = distribution(generator);
	}
	return index;
}

Aquarium::Aquarium() {
	if (hasInstance_) {
		std::cerr << "Cannot construct more than one instance of class 'Aquarium' !" << std::endl;
		assert(false);
	}
	hasInstance_ = !hasInstance_;
}

void Aquarium::addFish(std::string_view name, fishInfo::Sex sex, fishInfo::Species species) {
	fishes_.emplace_back(name, sex, species);
	std::cout << std::endl << "A fish (" << fishes_.back() << ") has been added ! Now the aquarium contains " << fishes_.size() << " !";
}

void Aquarium::addFish(std::initializer_list<Fish>&& fishes) {
	fishes_.reserve(fishes_.capacity() + fishes.size());
	for (auto&& fish : fishes) {
		fishes_.push_back(fish);
		std::cout << std::endl << "A fish (" << fishes_.back() << ") has been added ! Now the aquarium contains " << fishes_.size() << " !";
	}
}

void Aquarium::addAlga(std::size_t nAlgas) {
	algas_.reserve(algas_.capacity() + nAlgas);
	for (; nAlgas != 0u; nAlgas--) {
		algas_.emplace_back();
		std::cout << std::endl << "An alga has been added ! Now, the aquarium contains " << algas_.size() << " !";
	}
}

void Aquarium::turn(bool pauseBetweenEachStep) {
#ifdef _MSVC_LANG
#pragma warning(disable : 4820) // 3 bytes padding in lambda capture
#endif

	auto pauseIfNecesary{ 
		[pauseBetweenEachStep, this] {
			if (pauseBetweenEachStep) {
				pause();
			}
		}
	};

#ifdef _MSVC_LANG
#pragma warning(default : 4820)
#endif

	std::cout << std::endl << "========================================Turn " << nTurns_++ << " :========================================" << std::endl;
	std::cout << "Aquarium's state :" << std::endl << *this << newline;
	pauseIfNecesary();
	std::cout << "-------------------------------------------------Algas will get older !-------------------------------------------------" << std::endl;
	algasGrowingOlder();
	pauseIfNecesary();
	std::cout << "-------------------------------------------------Fishes will get older !------------------------------------------------" << std::endl;
	fishesGrowOlder();
	pauseIfNecesary();
	std::cout << "------------------------------------------------Fishes will be hungry !-------------------------------------------------" << std::endl;
	fishesHungry();
	pauseIfNecesary();
	std::cout << "---------------------------------------------------Algas will grow !----------------------------------------------------" << std::endl;
	algasGrowing();
	pauseIfNecesary();
	std::cout << "----------------------------------------Fishes and algas will try to reproduce !----------------------------------------" << std::endl;
	tryAlgaReproduction();
	pauseIfNecesary();
	tryFishReproduction();
	pauseIfNecesary();
	std::cout << "---------------------------------------------------Fishes will eat !----------------------------------------------------" << std::endl;
	fishesEat();
	pauseIfNecesary();
	std::cout << "------------------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << newline << "Lots of things happened, here are the new aquarium's states :" << std::endl << *this << newline;
	pauseIfNecesary();
}

void Aquarium::turn(std::size_t nTurns, bool pauseBetweenEachTurn, bool pauseBetweenEachStep) {
	for (; nTurns != 0u; nTurns--) {
		turn(pauseBetweenEachStep);
		if (pauseBetweenEachTurn) {
			pause();
		}
	}
}

std::ostream& operator<<(std::ostream& stream, const Aquarium& aquarium) {
	stream << "There ";
	switch (aquarium.algas_.size()) {
	case 0u:
		stream << "is no alga." << std::endl;
		break;
	case 1u:
		stream << "is only one alga." << std::endl;
		break;
	default:
		stream << "are " << aquarium.algas_.size() << " algas." << std::endl;
	}

	stream << "There ";
	switch (aquarium.fishes_.size()) {
	case 0u:
		return stream << "is no fish.";
	case 1u:
		return stream << "is only one fish : " << aquarium.fishes_[0];
	default:
		stream << "are " << aquarium.fishes_.size() << " fishes :" << std::endl;
		for (std::size_t index{ 0u }; const auto & fish : aquarium.fishes_) {
			stream << '\t' << fish;
			if (index != aquarium.fishes_.size() - 1u) { // doesn't display an endline after the last fish
				stream << std::endl;
			}
			index++;
		}
		return stream;
	}
}