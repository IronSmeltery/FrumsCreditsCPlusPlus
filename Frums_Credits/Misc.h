#ifndef MISC
#define MISC
#pragma warning(push)
#pragma warning(disable: 6031 4244 26819 26818)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOSCROLL
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
//I sincerely apologise for the include horror below
#include <SDL.h>
#include "SDL_events.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <functional>
#include <utility>
#include <conio.h>
#include <stdio.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <queue>
#include <numeric>
#include <unordered_set>
#include <typeindex>
#include <typeinfo>
#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#pragma warning(pop)
#include "Defines.h"
#include "ConsoleTypes.h"
#include "GlobalRNGDistributions.h"


//A collection of small stuff that is used in many places or is too obscure

template <typename T>
using SRef = std::shared_ptr<T>;
template <typename T>
using URef = std::unique_ptr<T>;
template <typename T>
using WRef = std::weak_ptr<T>;

//avoiding a whole library include by using this instead

template<typename T>
static inline T Clamp(T num, T min = T{ 0 }, T max = T{ 1 }) noexcept { return num > max ? max : (num < min ? min : num); }


using ColoredString = std::pair<strType, ConsoleTypes::Color>;
using ColoredChar = std::pair<charType, ConsoleTypes::Color>;

/*
value = min + percent * (max - min)
value - min = percent * (max - min)
(value - min)/(max - min) = percent
*/
static constexpr float Lerp(float percent, float min = 0, float max = 1) noexcept {
	return min + percent * (max - min);
}
static constexpr float Percent(float value, float min = 0, float max = 1) noexcept {
	return max == min ? max : (value - min) / (max - min);
}
static constexpr float MapRange(float value, float inMin, float inMax, float outMin, float outMax) {
	return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
}
/*
map range [a,b] to range [c,d]
subtract a        [0,b-a]
divide by (b-a)   [0,1]
multiply by (d-c) [0,d-c]
add c             [c,d]

y = ((x-a)/(b-a)) * (d-c) + c
*/




template <unsigned int size> class BoolBitArr {
	static_assert(size, "Size must be positive");
	class BoolByte {
		friend class BoolBitArr;
		bool b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1;
		bool operator[](char n) {
			switch (n) {
			case 0:return b0; case 1:return b1; case 2:return b2; case 3:return b3;
			case 4:return b4; case 5:return b5; case 6:return b6; case 7:return b7;
			}
		}
	};
	BoolByte arr[((size - 1) / 8) + 1];
public:bool operator[](unsigned long long n) { return(n <= size ? arr[n / 8][n % 8] : throw std::out_of_range("no")); }
};






// Adds delta to enum and return it as the enum type
template<typename T>
static inline T EnumDelta(T curr, int delta) noexcept { return static_cast<T>(static_cast<int>(curr) + delta); }

//#include <cstddef>

template<std::size_t N>
constexpr char at_literal(const char(&str)[N], std::size_t index) { return (index < N - 1) ? str[index] : throw std::out_of_range("C-string literal index out of range, size/indexed entry are:" + std::to_string(N) + "/" + std::to_string(index)); }
template<std::size_t N>
constexpr wchar_t at_literal(const wchar_t(&str)[N], std::size_t index) { return (index < N - 1) ? str[index] : throw std::out_of_range("C-string literal index out of range, size/indexed entry are:" + std::to_string(N) + "/" + std::to_string(index)); }

struct TimeData {
public:
	const float secondsPerBeat{};
	unsigned int beat{}, sceneStartBeat{0};
	double songTime{};
	TimeData()noexcept {};
	TimeData(const double songTime_, const float secondsPerBeat_)noexcept : songTime(songTime_), secondsPerBeat(secondsPerBeat_) {}
	// subdivision: 1=whole, 2=half, 4=quarter, 8=eighth, 16=sixteenth, etc.
	unsigned int BeatsElapsed(int offset = 0) const noexcept {
		return beat - sceneStartBeat + offset;
	}
	unsigned int SubdivIndex(unsigned int subdivision) const noexcept {
		return ToBeats(songTime, subdivision);
	}
	double SubdivFraction(unsigned int subdivision) const noexcept {
		return secondsPerBeat == 0 ? 0 : (songTime / secondsPerBeat) * subdivision;
	}
	double ToSongTime(unsigned int beats, unsigned int subdivision) const noexcept {
		return subdivision == 0 ? 0 : (static_cast<double>(beats) / static_cast<double>(subdivision)) * secondsPerBeat;
	}
	unsigned int ToBeats(double songTime_, unsigned int subdivision) const noexcept {
		return secondsPerBeat == 0 ? 0 : static_cast<unsigned int>(std::floor((songTime_ / secondsPerBeat) * subdivision));
	}
};
#define DBOUT_DEFINED
//Prints to the visual studio debug output, only works when running in the debugger.
static void DBOUT(std::string s) { 
#ifndef SHOWCASE_MODE
	OutputDebugStringA((s + '\n').c_str());
#endif
}
static void DBOUT(std::wstring s) { 
#ifndef SHOWCASE_MODE
	OutputDebugStringW((s + L'\n').c_str()); 
#endif
}


static bool ReadFileSegment(strType fileName, unsigned int segmentID, strType sectionDelimiter, std::vector<strType>& returnData, bool excludeDelimitedLine = true) {
	if (sectionDelimiter.empty()) return false;
	if (!std::filesystem::exists(fileName)) return false;
#ifdef useWideCharacters
	std::wifstream file(fileName);
#else
	std::ifstream file(fileName);
#endif

	if (!file.is_open()) return false;
	if (!file.good()) return false;
	strType buffer;
	unsigned int currSegment{ 0 };
	while (std::getline(file, buffer)) {
		if (buffer.compare(0, sectionDelimiter.size(), sectionDelimiter) == 0) {
			if (currSegment == segmentID)
				break;
			++currSegment;
		}
	}
	if (currSegment != segmentID) return false;
	returnData.clear();
	if (!excludeDelimitedLine) returnData.push_back(buffer);
	while (std::getline(file, buffer)) {
		if (buffer.compare(0, sectionDelimiter.size(), sectionDelimiter) == 0) {
			break;
		}
		returnData.push_back(buffer);
	}
	file.close();
	return true;
}
#ifdef useWideCharacters
static std::wstring ToXdp(double value, int decimalPlace = 1) {
	if (decimalPlace <= 0) decimalPlace = 1;
	std::wstring s = std::to_wstring(value);
	size_t dotPos = s.find('.');
	if (dotPos != std::wstring::npos && s.size() > dotPos + decimalPlace + 1) {
		s = s.substr(0, dotPos + decimalPlace + 1); // keep X decimals
	}
	return s;
}
#else
static std::string ToXdp(double value, int decimalPlace = 1) {
	if (decimalPlace <= 0) decimalPlace = 1;
	std::string s = std::to_string(value);
	auto dotPos = s.find('.');
	if (dotPos != std::string::npos && s.size() > dotPos + decimalPlace + 1) {
		s = s.substr(0, dotPos + decimalPlace + 1); // keep X decimals
	}
	return s;
}
#endif



static strType FormatTime(double value, int decimalPlace = 1, unsigned short fps = 60) {
	int total_secs_int = static_cast<int>(value);

	// Calculate time components
	//int hours = total_secs_int / 3600;
	int minutes = (total_secs_int % 3600) / 60;
	int seconds = total_secs_int % 60;

	// Format with leading zeros using stringstream
#ifdef useWideCharacters
	std::wstringstream ss;
	ss << std::setfill(L'0')
		//<< std::setw(2) << hours << L":"
		<< std::setw(2) << minutes << L":"
		<< std::setw(2) << seconds;
	if (decimalPlace > 0) {
		double fractional = value - total_secs_int;
		ss << L"." << std::setw(decimalPlace) << std::setfill(L'0') << static_cast<int>(fractional * fps);
	}
#else
	std::stringstream ss;
	ss << std::setfill('0')
		<< std::setw(2) << hours << ":"
		<< std::setw(2) << minutes << ":"
		<< std::setw(2) << seconds;
	if (decimalPlace > 0) {
		double fractional = value - total_secs_int;
		ss << "." << std::setw(decimalPlace) << std::setfill('0') << static_cast<int>(fractional * fps);
	}
#endif
	return ss.str();
}


template <typename T>
static strType to_strType(T n) {
	static_assert(std::is_arithmetic<T>::value, "Template type must be numeric!");
#ifdef useWideCharacters
	return std::to_wstring(n);
#else
	return std::to_string(n);
#endif
}


template<typename T, size_t N>
class iteration_limiter {
	size_t i = N;
public:
	T val;
	explicit operator bool() const noexcept { return --i > 0; };
};

template <typename T>
class AntiDuplicator {
	std::unordered_set<T> s;
public:
	AntiDuplicator(std::unordered_set<T> unwanted_values) : s(unwanted_values) {};

	static T no_dupes(std::function<T(void)> func, std::unordered_set<T> unwanted_values) {
		T v = func();
		while (s.count(v)) {
			v = func();
		}
		return v;
	}

	T no_dupes(std::function<T(void)> func) {
		return AntiDuplicator<T>::no_dupes(func, s);
	};
};

template <typename T>
class AntiDuplicateGenerator {
public:
	std::function<T(void)> func;
	std::vector<T> s;
	AntiDuplicateGenerator() {};
	AntiDuplicateGenerator(std::function<T(void)> function_to_get_value_from, size_t numElements, T defaultValue = T{ false }) :
		func(function_to_get_value_from) {
		s.reserve(numElements);
	};


	bool contains(T v) {
		if (s.capacity() == 0) return false;
		for (int i = 0; i < s.size(); i++) {
			if (s[i] == v) return true;
		}
		return false;
	}

	T generate_value() {
		if (func == nullptr) return T{ false };
		T v = func();
		while (contains(v)) {
			v = func();
		}

		if (s.size() < s.capacity()) {
			s.push_back(v);
		}
		else {
			std::rotate(s.begin(), s.begin() + 1, s.end());
			s.back() = v;
		}

		return v;
	};
};

static strType FuckUpString(strType str, float chance, SRef<RandomizerWithGlobals> globalRNG) {
	if (str.empty()) return str;
	const charType fucks[13] =
#ifdef useWideCharacters
		L"...  `=/?-$%";
#else
		"...  `=/?-$%";
#endif
	globalRNG->float_gen.ChangeDist(0.0f, 1.0f);
	globalRNG->int_gen.ChangeDist(0, 11);

	std::transform(str.begin(), str.end() - 1, str.begin(), [&globalRNG, &fucks, chance](charType c) {
		return (globalRNG->float_gen.Chance(chance) ? at_literal(fucks, globalRNG->int_gen.Rand()) : c);
		}
	);
	return str;
}


#endif