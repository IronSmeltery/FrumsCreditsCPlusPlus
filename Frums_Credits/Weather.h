#ifndef WEATHER_H
#define WEATHER_H
#include "Misc.h"
#include "Distributions.h"
#include "Console.h"
#include <cassert>
//22.10.2009 first
//06.11.2009 last
class Weather {
	AntiDuplicateGenerator<char> wind_history_{ [this]() {return Generate_Wind_Speed(); }, 5, -1 };
	//AntiDuplicateGenerator<char> temp_history_{ [this]() {return Generate_Temp(); }, 50, -1 };
	//std::array<char, 1+5> wind_history;
	SRef<RandomizerWithGlobals> randomizer{ nullptr };
	float precip{}, humidity{};
	char temp{}, wind_dir{}, wind_speed{};
	void Init(SRef<RandomizerWithGlobals> randomizer_, unsigned char _day, unsigned char _month, unsigned short _year);
public:
    //day and month index into arrays. subtract 1 from what number you would normally use to write it
	//months: 0 = january, 1 = february
	//days: 0 = 1st, 1 = 2nd, 2 = 3rd
	unsigned char day{}, month{};
	//year just takes the actual year number
	unsigned short year{};
	constexpr static int daysInMonth[12]{ 31,28,31,30,31,30,31,31,30,31,30,31 },
		daysPassedAtStartOfMonth[12]{ 0,31,59,90,120,151,181,212,243,273,304,334 };

	//Distribution<> day_rand; //[?, ?] for generating date numbers in any month using ChangeDistAndRand()
	Distribution<int_dist> day_30; //[0, 30] for generating date numbers in 31-day months
	Distribution<int_dist> day_29; //[0, 29] for generating date numbers in 30-day months
	Distribution<int_dist> day_27; //[0, 27] for generating date numbers in february
	Distribution<int_dist> month_11; //[0, 11] for generating months
	Distribution<int_dist> weather_gen_100; //[-100, 100] for generating a bunch of numbers in weather::update() just like in the original python code
	Distribution<int_dist> wind_dir_n3_3; //[-3, 3] for generating random wind directions
	Distribution<float_dist> humidity_n0p2_0p2; //[-0.2, 0.2] for generating the amount humidity changes by
	Distribution<float_dist> precip_n25_25; //[-0.25, 0.25] for generating the first amount precipitation changes by 
	Distribution<float_dist> wind_speed_n7p692_7p692; //[-7.692307692, 7.692307692] for generating the first amount wind speed changes by 
	Distribution<float_dist> temp_n5_5; //[-5.0, 5.0] for generating the first amount temperature changes by 
	
	Weather(SRef<RandomizerWithGlobals> randomizer_);
	Weather(SRef<RandomizerWithGlobals> randomizer_, unsigned char _day, unsigned char _month, unsigned short _year);

	void Update();
	void RandomizeDayMonth();
	void IncrementDay();

	//1 to 3 chars with "°F" after, dont add spaces, can be negative ("-12")
	ColoredString GetTemp();
	//needs exactly 2 chars returned, put space in front if only 1
	std::wstring GetSpeed();
	//left aligned, can be 1 or 2 chars, dont put space infront
	std::wstring GetDir();
	std::wstring GetWindLine();
	//dont move the %'s position, number is right aligned from it (note for using the string this returns)
	std::wstring GetPrecipt();
	std::wstring GetWeatherType() const;
	std::wstring GetDate();
	void Print() {};
private:
	char Generate_Wind_Speed();
	char Generate_Temp();
    int rand_int(int a, int b);
    float rand_float(float a, float b);
};

//22.10.2009: Cloudy,		precip 20%, temp 43, wind dir 6, 13 ([gust]25) W
//23.10.2009: Sunny,		precip 4%,	temp 52, wind dir 7, 12 ([gust]25) NW
//24.10.2009: Partly sunny, precip 7%,	temp 48, wind dir 1, 8	([gust]20) N 
//25.10.2009 and beyond: randomly mutate value.
//Prefer 33% precip, move wind dir by +-3 each day, gust = speed x 2.1 - 2.6

#endif // WEATHER_H


