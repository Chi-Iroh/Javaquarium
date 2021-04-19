#include "Fish.hpp"

#ifdef _MSVC_LANG
#pragma warning(push, 0)
#endif

#include <iostream>
#include <cassert>
#include <type_traits>
#include <chrono>
#include <random>
#include <array>

#ifdef _MSVC_LANG
#pragma warning(pop)
#endif

void LivingBeing::getDamage(std::size_t damages) noexcept {
	HP_ = (damages >= HP_) ? 0u : HP_ - damages;
}

void LivingBeing::getOlder() {
	if (++age_ == 20u) {
		throw std::runtime_error("Too old !");
	}
}

std::size_t randomLivingBeingAge() {
	static std::random_device device{};
	static const auto firstCallTime{ std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() };
	std::mt19937 generator{
		static_cast<std::mt19937::result_type>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - firstCallTime
		)
	};
	// size is checked in fishEat function; not static because size may change between two calls
	std::uniform_int_distribution<std::size_t> distribution{ 0u, LivingBeing::maxAge - 1u }; // cannot buy a dead fish
	return (distribution(generator) * (distribution(generator) ^ distribution(generator))) % 19u;
}

Alga::Alga(std::size_t age, std::size_t HP) noexcept {
	if (age > 19u) {
		std::cerr << "Cannot buy a dead alga ! Age must be in [0;19] (" << age << " given ) !" << std::endl;
		assert(false);
	}
	age_ = age;
	if (HP == 0u) {
		std::cerr << "Cannot buy a dead alga ! HP must be greater than 0 !" << std::endl;
		assert(false);
	}
	HP_ = HP;
}

bool fishInfo::isCarnivorous(Species species) noexcept {
	return species == Species::grouper || species == Species::tuna || species == Species::clownfish;
}

bool fishInfo::isHermaphroditicAge(Species species) noexcept {
	return reproductionTable.at(species) == ReproductionMode::hermaphroditicAge;
}

bool fishInfo::isHermaphroditicOpportunistic(Species species) noexcept {
	return reproductionTable.at(species) == ReproductionMode::hermaphroditicOpportunistic;
}

fishInfo::Sex fishInfo::randomSex() {
	return (randomLivingBeingAge() % 2 == 0u) ? Sex::male : Sex::female;
}

fishInfo::Sex fishInfo::operator!(Sex sex) noexcept {
	return (sex == Sex::male) ? Sex::female : Sex::male;
}

std::ostream& fishInfo::operator<<(std::ostream& stream, Sex sex) {
	return stream << ((sex == Sex::male) ? "male" : "female");
}

std::ostream& fishInfo::operator<<(std::ostream& stream, Species species) {
	switch (species) {
	case Species::grouper:
		return stream << "grouper";
	case Species::tuna:
		return stream << "tuna";
	case Species::clownfish:
		return stream << "clownfish";
	case Species::sole:
		return stream << "sole";
	case Species::bass:
		return stream << "bass";
	case Species::carp:
		return stream << "carp";
	}
	return stream;
}

void Fish::checkSexForHermaphroditicAge() noexcept {
	if (fishInfo::reproductionTable.at(species_) == fishInfo::ReproductionMode::hermaphroditicAge) {
		if (age_ < 10u) { // must be male
			if (sex_ != fishInfo::Sex::male) {
				std::cerr << "A fish hermaphroditic with the age must be a male from 0 year to 9 years old included !" << std::endl;
				assert(false);
			}
		}
		else { // must be a female
			if (sex_ != fishInfo::Sex::female) {
				std::cerr << "A fish hermaphroditic with the age must be a female from 10 years to 19 years old included !" << std::endl;
				assert(false);
			}
		}
	}
}

Fish::Fish(std::string_view name, fishInfo::Sex sex, fishInfo::Species species, std::size_t age) noexcept : name_(name), species_(species) {
	if (age > 19u) {
		std::cerr << "Cannot buy a dead fish ! Age must be in [0;19] (" << age << " given) !" << std::endl;
		assert(false);
	}
	age_ = age;
	sex_ = sex;

	checkSexForHermaphroditicAge();
}

Fish::Fish(std::string_view name, fishInfo::Sex sex, fishInfo::Species species) noexcept : name_(name), sex_(sex), species_(species) {
	checkSexForHermaphroditicAge();
}

Fish::Fish(std::string_view name, fishInfo::Species species, std::size_t age) noexcept {
	if (fishInfo::reproductionTable.at(species) != fishInfo::ReproductionMode::hermaphroditicAge) {
		std::cerr << "Constructor 'Fish(std::string_view name, fishInfo::Species species, std::size_t age) noexcept' is only usable with a species which is hermaphroditic with the age because the sex will be deduced automatically !" << std::endl;
		assert(false);
	}
	name_ = name;
	species_ = species;
	if (age > 19u) {
		std::cerr << "Cannot buy a dead fish ! Age must be in [0;19] (" << age << " given) !" << std::endl;
		assert(false);
	}
	age_ = age;
	sex_ = (age_ < 10u) ? fishInfo::Sex::male : fishInfo::Sex::female;
}

Fish::Fish(Fish&& other) noexcept : name_(other.name_), species_(other.species_) {
	sex_ = other.sex_;
	age_ = other.age_;
	checkSexForHermaphroditicAge();

	HP_ = other.HP_;

	other.HP_ = 0u;
	other.age_ = 0u;
	other.name_ = std::string();
	other.sex_ = fishInfo::Sex();
	other.species_ = fishInfo::Species();
}

Fish::Fish(const Fish& other) noexcept : name_(other.name_), sex_(other.sex_), species_(other.species_) {
	age_ = other.age_;
	checkSexForHermaphroditicAge();

	HP_ = other.HP_;
}

Fish& Fish::operator=(const Fish& other) noexcept {
	age_ = other.age_;
	checkSexForHermaphroditicAge();

	HP_ = other.HP_;
	name_ = other.name_;
	sex_ = other.sex_;
	species_ = other.species_;
	return *this;
}

Fish& Fish::operator=(Fish&& other) noexcept {
	HP_ = other.HP_;
	name_ = other.name_;
	sex_ = other.sex_;
	species_ = other.species_;
	age_ = other.age_;

	checkSexForHermaphroditicAge();

	other.HP_ = 0u;
	other.age_ = 0u;
	other.name_ = std::string();
	other.sex_ = fishInfo::Sex();
	other.species_ = fishInfo::Species();
	return *this;
}

bool Fish::isCarnivorous() const noexcept {
	return fishInfo::isCarnivorous(species_);
}

void Fish::getHungry() noexcept {
	getDamage(1u);
}

void Fish::getOlder() {
	if (++age_ == 20u) {
		throw std::runtime_error("Too old !");
	}
	if (fishInfo::isHermaphroditicAge(species_)) {
		if (age_ == 10u) {
			std::cout << std::endl << name_ << " now has 10 years and becomes a female !";
			sex_ = fishInfo::Sex::female;
		}
	}
}

void Fish::eat(Alga& target) noexcept {
	if (isCarnivorous()) {
		std::cerr << *this << " isn't herbivorous and cannot eat an alga !" << std::endl;
		assert(false);
	}
	if (!needsToEat()) {
		std::cout << name_ << " isn't hungry !" << std::endl;
		return;
	}
	static constexpr std::size_t algaDamageByHerbivorous{ 2u };
	static constexpr std::size_t algaHealthGivenToHerbivorous{ 3u };
	if (target.HP_ <= algaDamageByHerbivorous) {
		target.HP_ = 0u;
	}
	else {
		target.HP_ -= algaDamageByHerbivorous;
	}
	HP_ += algaHealthGivenToHerbivorous;
	std::cout << std::endl << name_ << " eats an alga, recovers " << algaHealthGivenToHerbivorous << " HP and has now " << HP_ << " HP !" << std::endl;
	std::cout << "The alga losts " << algaDamageByHerbivorous << " HP and has now " << target.HP_ << " HP !";
	if (target.HP_ == 0u) {
		std::cout << std::endl << "An alga died eaten by " << name_ << " !";
	}
	std::cout << newline;
}

void Fish::eat(Fish& target) noexcept {
	if (!isCarnivorous()) {
		std::cerr << *this << " isn't carnivorous and cannot eat " << target << std::endl;
		assert(false);
	}
	if (species_ == target.species_) {
		std::cerr << *this << " cannot eat " << target << " because they belong ot the same species !" << std::endl;
		assert(false);
	}
	if (!needsToEat()) {
		std::cout << name_ << " isn't hungry !" << std::endl;
		return;
	}
	static constexpr std::size_t fishDamagebyCarnivorous{ 4u };
	static constexpr std::size_t fishHealthGivenToCarnivorous{ 5u };
	if (target.HP_ <= fishDamagebyCarnivorous) {
		target.HP_ = 0u;
	}
	else {
		target.HP_ -= fishDamagebyCarnivorous;
	}
	HP_ += fishHealthGivenToCarnivorous;
	std::cout << std::endl << name_ << " eats " << target.name_ << ", recovers " << fishHealthGivenToCarnivorous << " HP and has now " << HP_ << " HP !" << std::endl;
	std::cout << target.name_ << " losts " << fishDamagebyCarnivorous << " HP and has now " << target.HP_ << " HP !";
	if (target.HP_ == 0u) {
		std::cout << std::endl << "An alga died eaten by " << name_ << " !";
	}
	std::cout << newline;
}

bool Fish::canReproduceWith(const Fish& other) const noexcept {
	if (species_ != other.species_) {
		return false;
	}
	if (fishInfo::isHermaphroditicOpportunistic(species_)) {
		return true; // can change his/her sex
	}
	return sex_ != other.sex_;
}

Fish Fish::reproduceWith(const Fish& other, fishInfo::Sex childSex, std::string_view childName) {
	if (species_ != other.species_) {
		std::cerr << std::endl << *this << " cannot reproduce " << ((sex_ == fishInfo::Sex::male) ? "himself" : "herself") << " with " << other << " because they don't belong to the same species !" << std::endl;
		assert(false);
	}
	if (fishInfo::isHermaphroditicOpportunistic(species_)) {
		if (sex_ == other.sex_) {
			sex_ = !sex_;
			std::cout << std::endl << name_ << " is hermaphroditic opportunistic and becomes now a " << sex_ << " !";
		}
	}
	if (sex_ == other.sex_) {
		std::cerr << std::endl << *this << " cannot reproduce " << ((sex_ == fishInfo::Sex::male) ? "himself" : "herself") << " with " << other << " because they've the same sex !" << std::endl;
		assert(false);
	}
	if (fishInfo::isHermaphroditicAge(species_)) {
		return Fish(childName, species_, 0u); // sex deduced
	}
	return Fish(childName, childSex, species_, 0u);
}

std::ostream& operator<<(std::ostream& stream, const Fish& fish) noexcept {
	stream << fish.name_ << " : " << fish.sex_ << ' ' << fish.species_ << " has " << fish.HP_ << " HP and is ";
	switch (fish.age_) {
	case 0u:
		return stream << "0 year old !";
	case 1u:
		return stream << "only one year old !";
	default:
		return stream << fish.age_ << " years old !";
	}
}