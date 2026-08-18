#include "Weather.h"
Weather::Weather(SRef<RandomizerWithGlobals> randomizer_) { Init(randomizer_, 0, 0, 2009); }
Weather::Weather(SRef<RandomizerWithGlobals> randomizer_, unsigned char _day, unsigned char _month, unsigned short _year) { Init(randomizer_, _day, _month, _year); }



void Weather::Init(SRef<RandomizerWithGlobals> randomizer_, unsigned char _day, unsigned char _month, unsigned short _year) {
	day = _day;
	month = _month;
	year = _year;
	randomizer = randomizer_;
	//day_rand = IntDistribution(*randomizer_, 0, 30); 
	day_30 = Distribution<int_dist>(*randomizer_, 0, 30);
	day_29 = Distribution<int_dist>(*randomizer_, 0, 29);
	day_27 = Distribution<int_dist>(*randomizer_, 0, 27);
	month_11 = Distribution<int_dist>(*randomizer_, 0, 11);
	weather_gen_100 = Distribution<int_dist>(*randomizer_, -100, 100);
	wind_dir_n3_3 = Distribution<int_dist>(*randomizer_, -3, 3);
	humidity_n0p2_0p2 = Distribution<float_dist>(*randomizer_, -0.2f, 0.2f);
	precip_n25_25 = Distribution<float_dist>(*randomizer_, -25.0f, 25.0f);
	wind_speed_n7p692_7p692 = Distribution<float_dist>(*randomizer_, -7.692307692f, 7.692307692f);
	temp_n5_5 = Distribution<float_dist>(*randomizer_, -5.0f, 5.0f);
};
void Weather::Update() {
	if (year == 2009 && month == 9) {
		if (day == 21) { wind_speed = 13, wind_dir = 6, precip = 20.30001f, temp = 43, humidity = 0.66f; Print(); return; }
		else if (day == 22) { wind_speed = 12, wind_dir = 7, precip = 4.0f, temp = 52, humidity = 0.10f; Print(); return; }
		else if (day == 23) { wind_speed = 8,  wind_dir = 2, precip = 7.0f, temp = 48, humidity = 0.10f; Print(); return; }
	}

	//rando \/

	/*
	things we actually need to generate at the end of the day
	precip
	- [-0.25, 0.25]
	- [0, (0.5 * abs(0.33 - self.precip))]
	wind_dir
	- [-3, 3]
	wind_speed
	- [-7.692307692, 7.692307692]
	- [0, abs(15 - self.wind)]/3 or [0, abs(15 - self.wind)/3] or [0, abs(5 - self.wind/3)]
	temp
	- [-5.0, 5.0]
	- [0, abs(temp_target - self.temp)]/5 or [0, abs(temp_target - self.temp)/5] or [0, abs(temp_target/5 - self.temp/5)]
	humidity
	- [-0.2, 0.2]
	- [0, abs(0.2 - self.humidity)]/200 or [0, abs(0.2 - self.humidity)/200] or [0, abs(0.001 - self.humidity/200)]


	*/

	precip = std::min(100.0f, std::max(0.0f, precip + precip_n25_25.Rand() +
		(rand_float(0.0f, (0.5f * abs(33.0f - precip)))) * (33.0f - precip < 0 ? -1 : 1)));
	//precip = std::min(100.0f, std::max(0.0f, precip + weather_gen_100.Rand() / 400.0f +
	//	(rand_int(0, (int)(100 * abs(0.33f - precip))) / 200.0f) * (0.33f - precip < 0 ? -1 : 1)));

	//wind_dir += (rand_int(-3, 3) + 8);
	wind_dir += wind_dir_n3_3.Rand() + 8;
	wind_dir %= 8;

	//wind_speed += wind_speed_n7p692_7p692.Rand();
	//wind_speed += rand_int(0, abs(15.0f - wind_speed)) / 3.0f) * (15.0f - wind_speed < 0 ? -1 : 1);
	//if (wind_speed < 0) {wind_speed *= -1;}
	//if (wind_speed < 0) {wind_speed = 0;}
	//
	//	wind_speed = max(0.0f, wind_speed + weather_gen_100.Rand() / 13.0f + (rand_int(0, abs(15.0f - wind_speed)) / 3.0f) * (15.0f - wind_speed < 0 ? -1 : 1));
	wind_speed = wind_history_.generate_value();
	//for (bool same = true; same;) {
	//	wind_speed = max(0.0f, wind_speed + weather_gen_100.Rand() / 13.0f + (rand_int(0, abs(15.0f - wind_speed)) / 3.0f) * (15.0f - wind_speed < 0 ? -1 : 1));
	//	same = false;
	//	for (int i = 1; i < wind_history.size(); i++) {
	//		if (wind_speed == wind_history[i]) {
	//			same = true;
	//			break;
	//		}
	//	}
	//}
	//wind_history[wind_arr_index] = wind_speed;
	//wind_arr_index = ((wind_arr_index + 1) % wind_history.size()) + 1;
	//gust = wind_speed*rand_int(210, 260)*0.01f;


	//for (int i = 0; i < month and i < 12; i++) { day_in_year += daysInMonth[i]; }

	temp = Generate_Temp();// temp_history_.generate_value();

	//humidity += weather_gen_100.Rand() / 500.0f;
	humidity += humidity_n0p2_0p2.Rand();
	//humidity += float_gen.ChangeDistAndRand(-0.2f, 0.2f);
	humidity = Clamp(humidity);

	//Print();

}

std::pair<std::wstring, ConsoleTypes::Color> Weather::GetTemp() {
	std::pair<std::wstring, ConsoleTypes::Color> ret;

	ret.first = std::to_wstring(temp) + L"°F  ";
	if (temp <= 23) { ret.second = ConsoleTypes::Color::F_WHITE_BRIGHT; }
	else if (temp <= 46) { ret.second = ConsoleTypes::Color::F_CYAN_BRIGHT; }
	else if (temp <= 69) { ret.second = ConsoleTypes::Color::F_YELLOW_BRIGHT; }
	else { ret.second = ConsoleTypes::Color::F_RED_BRIGHT; }
	return ret;
}

//placeholder //needs exactly 2 chars returned, put space in front if only 1
std::wstring Weather::GetSpeed() {
	return (wind_speed < 10 ? L" " : L"") + std::to_wstring(wind_speed);
}

//placeholder   //left aligned, can be 1 or 2 chars, dont put space infront
std::wstring Weather::GetDir() {
	switch (wind_dir) {
	case 0: return L"N ";
	case 1: return L"NE";
	case 2: return L"E ";
	case 3: return L"SE";
	case 4: return L"S ";
	case 5: return L"SW";
	case 6: return L"W ";
	case 7: return L"NW";
	default: return L"!!";
	}
}

std::wstring Weather::GetWindLine() { return L"Wind " + GetSpeed() + L" mph " + GetDir(); }

//dont add spaces to move the position, it gets center aligned in the scene
std::wstring Weather::GetPrecipt() {
	std::wstring str = ToXdp(precip, 2);
	if (str == L"0.00") { 
		return L"0%"; }
	size_t deci = str.find(L'.');
	if (str[deci+2] == L'0') { 
		str = str.substr(0, deci+2); }
	return str + L"%";
}

std::wstring Weather::GetWeatherType() const {
	std::wstring str;
	if (humidity > 0.5f) {
		if (precip > 0.4f) {
			if (wind_speed > 43.0f) str = temp < 32.0f ? L"Blizzard" : L"Hurricane";
			else if (wind_speed > 25.0f) str = temp < 32.0f ? L"Snowstorm" : L"Thunderstorm";
			else str = temp < 32.0f ? L"Snow" : L"Rain";
		}
		else if (precip > 0.25f) str = temp < 32.0f ? L"Sleet" : L"Drizzle";
		else {
			if (humidity > 0.8f || precip > 0.5) str = L"Overcast";
			else if (humidity > 0.65f || precip > 0.3f) str = L"Cloudy";
			else str = L"Partly sunny";
		}
	}
	else {
		if (precip > 0.4f) str = temp < 32.0f ? L"Snow" : L"Rain";
		else if (precip > 0.2f) str = temp < 32.0f ? L"Sleet" : L"Drizzle";
		str = humidity < 0.2f ? humidity < 0.1f ? L"Sunny" : L"Partly Sunny" : L"Clear";
	}
	return std::wstring(15 - str.length(), L' ') + str;
}

std::wstring Weather::GetDate() {
	return ((day < 9) ? L"0" : L"") + std::to_wstring(day + 1) + L"." +
		((month < 9) ? L"0" : L"") + std::to_wstring(month + 1) + L"." + std::to_wstring(year);
}

void Weather::RandomizeDayMonth() {
	month = month_11.Rand();
	//day = day_rand.ChangeDistAndRand(0, daysInMonth[month]-1);
	switch (daysInMonth[month]) {
	case 28: day = day_27.Rand(); return;
	case 30: day = day_29.Rand(); return;
	case 31: day = day_30.Rand(); return;
	default: day = 0; throw "wtf";
	}
}

void Weather::IncrementDay() {
	day++;
	day %= daysInMonth[month];
	if (day == 0) {
		month++;
		month %= 12;
		if (month == 0) {
			year++;
		}
	}
}
char Weather::Generate_Wind_Speed() {
	return (char)std::max(0.0f, (float)(wind_speed + wind_speed_n7p692_7p692.Rand() + (rand_int(0, abs(15 - wind_speed)) * 0.33333333f) * (15 - wind_speed < 0 ? -1 : 1)));
	//return (char)std::max(0.0f, (float)(wind_speed + weather_gen_100.Rand() / 13.0f + (rand_int(0, abs(15 - wind_speed)) / 3) * (15 - wind_speed < 0 ? -1 : 1)));
}

char Weather::Generate_Temp() {
	int day_in_year = day + daysPassedAtStartOfMonth[month];
	float temp_target = 40.0f * (1.0f + sin(((float)M_PI * 2.0f * (day_in_year - 21)) / 365.0f - (float)M_PI / 3.0f)) + 20.0f;
	//auto r1 = weather_gen_100.Rand();
	//auto v1 = r1 / 20.0f;
	auto v1 = temp_n5_5.Rand();
	auto abs1 = abs(temp_target - temp);
	//auto r2 = (weather_gen_100.Rand()+100)*static_cast<int>((abs1 - 0)/200.0f) + 0;
	auto r2 = rand_float(0.0f, abs1);
	auto v2 = r2 / 5.0f;
	auto v3 = (temp_target - temp < 0 ? -1 : 1);
	auto diff = v1 + v2 * v3;
	char test = (char)std::max(0.0f, temp + diff);
	/*
	char test = (char)std::max(0.0f, temp + weather_gen_100.Rand() / 20.0f +
		(rand_int(0, abs(temp_target - temp)) / 5.0f) * (temp_target - temp < 0 ? -1 : 1));;
	*/
	return test;
}

int Weather::rand_int(int a, int b) {
	return a == b ? a : randomizer->int_gen.ChangeDistAndRand(a, b);
	//return static_cast<int>((weather_gen_100.Rand() + 100) / 200.0f * (b - a) + a);
}

float Weather::rand_float(float a, float b) {
	return a == b ? a : randomizer->float_gen.ChangeDistAndRand(a, b);
	//return static_cast<int>((weather_gen_100.Rand() + 100) / 200.0f * (b - a) + a);
}

