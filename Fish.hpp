#ifdef _MSVC_LANG
#pragma warning(push, 0)
#endif

#include <string>
#include <iosfwd>
#include <map>

#ifdef _MSVC_LANG
#pragma warning(pop)
#endif

namespace fishInfo {
	enum class Sex { male, female};

	Sex randomSex();

	enum class Species {
		// (mérou) carnivorous
		grouper,
		// (thon) carnivorous
		tuna,
		// carnivorous
		clownfish,
		// herbivorous
		sole,
		// (bar) herbivorous
		bass,
		// herbivorous
		carp
	};

	enum class ReproductionMode {
		monoSex,
		hermaphroditicAge,
		hermaphroditicOpportunistic,
	};

	const std::map<Species, ReproductionMode> reproductionTable{
		{Species::carp, ReproductionMode::monoSex},
		{Species::tuna, ReproductionMode::monoSex},

		{Species::bass, ReproductionMode::hermaphroditicAge},
		{Species::grouper, ReproductionMode::hermaphroditicAge},

		{Species::sole, ReproductionMode::hermaphroditicOpportunistic},
		{Species::clownfish, ReproductionMode::hermaphroditicOpportunistic},

	};

	bool isCarnivorous(Species species) noexcept;

	bool isHermaphroditicAge(Species species) noexcept;

	bool isHermaphroditicOpportunistic(Species species) noexcept;

	Sex operator!(Sex sex) noexcept;

	std::ostream& operator<<(std::ostream& stream, Species species);

	std::ostream& operator<<(std::ostream& stream, Sex sex);
}

#define newline std::endl << std::endl

std::size_t randomLivingBeingAge();

class LivingBeing {
protected:
	std::size_t HP_{ 10u };
	std::size_t age_{ randomLivingBeingAge() };

public:

	void getDamage(std::size_t damages) noexcept;

	void getOlder();

	inline std::size_t HP() const noexcept {
		return HP_;
	}

	inline std::size_t age() const noexcept {
		return age_;
	}

	static constexpr std::size_t maxAge{ 20u }; // read in Aquarium class
};

class Alga : public LivingBeing {
	friend class Fish;

public:
	Alga(std::size_t age = 0u, std::size_t HP = 10u) noexcept;

	inline void grow() noexcept {
		HP_++;
	}
};

#ifdef _MSVC_LANG
#pragma warning(disable : 4820) // 3 bytes padding
#endif

class Fish : public LivingBeing {
	std::string name_{};
	fishInfo::Sex sex_{};
	// diet will be implicitly deduced
	fishInfo::Species species_{};
	static constexpr std::size_t fishHungryHP_{ 5u };

	void checkSexForHermaphroditicAge() noexcept;

public:
	Fish(std::string_view name, fishInfo::Sex sex, fishInfo::Species species, std::size_t age) noexcept;

	Fish(std::string_view name, fishInfo::Sex sex, fishInfo::Species species) noexcept;

	Fish(std::string_view name, fishInfo::Species species, std::size_t age = randomLivingBeingAge()) noexcept;

	Fish(Fish&& other) noexcept;

	Fish(const Fish& other) noexcept;

	Fish& operator=(const Fish& other) noexcept;

	Fish& operator=(Fish&& other) noexcept;

	bool isCarnivorous() const noexcept;

	void getHungry() noexcept;

	void getOlder(); // may throw if fish's dead

	inline std::string name() const noexcept {
		return name_;
	}

	inline bool needsToEat() const noexcept {
		return HP_ <= fishHungryHP_;
	}

	void eat(Alga& target) noexcept;

	void eat(Fish& target) noexcept;

	Fish reproduceWith(const Fish& other, fishInfo::Sex childSex, std::string_view childName);

	bool canReproduceWith(const Fish& other) const noexcept;

	inline fishInfo::Species species() const noexcept {
		return species_;
	}

	friend std::ostream& operator<<(std::ostream& stream, const Fish& fish) noexcept;
};

#ifdef _MSVC_LANG
#pragma warning(default : 4820)
#endif