#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H
#include "Randomizer.h"
#include <random>
#undef max
#undef min
//----------
template <typename DIST>
struct Distribution_Base {
	//template <typename T = size_t, std::enable_if_t<std::is_arithmetic_v<T>, T>> Rand_Base(DIST dist) {
	template <typename CastType = typename DIST::result_type,
		typename = std::enable_if_t<std::is_arithmetic_v<CastType> && !std::is_same_v<CastType, bool>>>
		inline CastType Rand_Base(DIST dist) {
			//using rt = typename DIST::result_type;
			return static_cast<CastType>(dist(rng->rng));
		}
protected:
	Randomizer* rng{ nullptr };
	Distribution_Base() = default;
	Distribution_Base(Randomizer& rng_) : rng(&rng_) {}
};
//----------
struct Distribution_Funcs {
	template <typename DIST, typename RetType = typename DIST::result_type>
	static inline RetType Rand_(Distribution_Base<DIST>& rng, DIST& dist) {
		return rng.Rand_Base(dist);
	}
	template <typename DIST, typename RetType = bool>
	static inline RetType Chance_(Distribution_Base<DIST>& rng, DIST& dist, float percent, RetType trueVal, RetType falseVal) {
		return (percent && (rng.Rand_Base(dist) - dist.min() <= percent * (dist.max() - dist.min()))) ? trueVal : falseVal;
	}
	template <typename DIST>
	static inline bool Chance_(Distribution_Base<DIST>& rng, DIST& dist, float percent) {
		return (percent && (rng.Rand_Base(dist) - dist.min() <= percent * (dist.max() - dist.min())));
	}
};

//----------
template <typename N = int, typename DIST = std::uniform_int_distribution<N>>
struct Distribution : Distribution_Base<DIST> {
	DIST dist;
	Distribution(Randomizer& rng_, N min, N max) : Distribution_Base<DIST>(rng_), dist(DIST(min, max)) {
		if (min >= max) throw std::invalid_argument("min cannot be greater or equal to max");
	}
	Distribution() = default;
	void ChangeDist(N min, N max) {
		if (min >= max) throw std::invalid_argument("min cannot be greater or equal to max");
		dist.param(DIST::param_type(min, max));
	}
	template <typename RetType = N> inline RetType Rand() {
		return Distribution_Funcs::Rand_<DIST, RetType>(*this, dist);
	}
	template <typename RetType = N> inline RetType ChangeDistAndRand(N min, N max) {
		ChangeDist(min, max); 
		return Rand<RetType>();
	}
	template <typename RetType> inline RetType Chance(float percent, RetType trueVal = RetType{ 1 }, RetType falseVal = RetType{ 0 }) {
		return Distribution_Funcs::Chance_<DIST, RetType>(*this, dist, percent, trueVal, falseVal);
	}
	inline bool Chance(float percent) {
		return Distribution_Funcs::Chance_<DIST>(*this, dist, percent);
	}
	template <typename RetType> inline RetType ChangeDistAndChance(N min, N max, float percent, RetType trueVal = RetType{ 1 }, RetType falseVal = RetType{ 0 }) {
		ChangeDist(min, max); 
		if (typeid(RetType) == typeid(bool)) return Chance(percent);
		return Chance(percent, trueVal, falseVal);
	}
	inline bool ChangeDistAndChance(N min, N max, float percent) {
		ChangeDist(min, max);
		return Chance(percent);
	}
};
#endif