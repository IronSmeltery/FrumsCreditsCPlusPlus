#ifndef RANDOMIZER
#define RANDOMIZER
#include <random>
#ifndef RNG_ENGINE
#define RNG_ENGINE std::minstd_rand
#endif

struct Randomizer {
	template<typename T, typename M = int>
	static M MakeSeed(const T& hashableObj) {
		std::size_t h = std::hash<T>{}(hashableObj);
		return static_cast<M>(h ^ (h >> 32));
	}
	RNG_ENGINE rng;
	Randomizer() {};
	Randomizer(int seed) : rng(seed) {};
};

#endif