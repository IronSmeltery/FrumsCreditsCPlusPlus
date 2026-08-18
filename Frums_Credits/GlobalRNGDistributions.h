#ifndef GLOBAL_RNG_DISTRIBUTIONS
#define GLOBAL_RNG_DISTRIBUTIONS
#include "Randomizer.h"
#include "Distributions.h"
struct RandomizerWithGlobals : public Randomizer {
#define realDist(a) a, std::uniform_real_distribution<a>
#define intDist(a) a, std::uniform_int_distribution<a>
#define double_dist realDist(double)
#define float_dist realDist(float)
#define long_long_dist intDist(long long)
#define long_dist intDist(long)
#define int_dist intDist(int)
#define short_dist intDist(short)
	Distribution<double_dist> double_gen;
	Distribution<float_dist> float_gen;
	Distribution<long_long_dist> long_long_gen;
	Distribution<long_dist> long_gen;
	Distribution<int_dist> int_gen;
	Distribution<short_dist> short_gen;
	RandomizerWithGlobals(int seed) : Randomizer(seed) {
		double_gen = Distribution<double_dist>(*this, -1.0, 1.0);
		float_gen = Distribution<float_dist>(*this, -1.0f, 1.0f);
		long_long_gen = Distribution<long_long_dist>(*this, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max());
		long_gen = Distribution<long_dist>(*this, std::numeric_limits<long>::min(), std::numeric_limits<long>::max());
		int_gen = Distribution<int_dist>(*this, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
		short_gen = Distribution<short_dist>(*this, std::numeric_limits<short>::min(), std::numeric_limits<short>::max());
	}
};
#endif