#ifndef SCENES
#define SCENES
#include "Misc.h"
#include "Console.h"
#include "Distributions.h"
#include "Ocean.h"
#include "Typewriter.h"
#include "CenterBox.h"
#include "Weather.h"
#include "HistoryLog.h"
#include "StaggeredWriter.h"
#include "SceneData.h"
#include "ConsoleExtensions.h"

#ifdef test_for_debug_removals 
#error test_for_debug_removals
#endif
#include <bitset>





enum stringLabels : short {
	S2_ocean_1 = 0,
	S2_ocean_2,
	S2_ocean_3,
	S2_ocean_4,
	S3_rising_weather,
	S4_loading_bar,
	S5_diagnosis,
	S5_searching,
	S5_date,
	S6_ocean_5,
	S7_access_points,
	S8_outro,
	enum_count,
};
#ifndef defaultTimeSubDiv
#define defaultTimeSubDiv 8
#endif
class SceneBase {
protected:
	SRef<Console> console{ nullptr };
	SRef<RandomizerWithGlobals> randomizer{ nullptr };
	SRef<TimeData> time{ nullptr };
public:
	enum debugControls {
		db_tilde = 0,
		db_1,
		db_2,
		db_3,
		db_4,
		db_5,
		db_6,
		db_7,
		db_8,
		db_9,
		db_0,
		db_min,
		db_equal,
		ENUM_END,
	};
	std::bitset<ENUM_END> debugBits;
	bool finished{ false }, overrideTitleBar{ false };
	unsigned char skipIndex{ 0 };
	virtual double StartTime() = 0;
	virtual int TimeSubDiv() { return defaultTimeSubDiv; };
#define SceneConstructorDataWithTypes SRef<Console> console, SRef<RandomizerWithGlobals> randomizer, SRef<TimeData> time, unsigned int skipIndex
#define SceneConstructorDataWithoutTypes console, randomizer, time, skipIndex
	SceneBase(SceneConstructorDataWithTypes) : console(console), randomizer(randomizer), time(time), skipIndex(skipIndex){}
	virtual void Init() = 0;
	virtual bool Run() = 0;
	virtual std::wstring extraTitleBarInfo() { return L""; }
};

class Scene_1_Static : public SceneBase {
	Distribution<int_dist> symbol_3, screenH_24, screenW_40, symbolColor;
public:
	double StartTime() override { return S1_SongStart; }
	Scene_1_Static(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
	void WriteNoise(int amount, int white, int light_grey, int dark_grey);
};
class Scene_2_Weather_1 : public SceneBase {
	short textBlockID{ enum_count };
	const short textBoxSizeX{ nScreenWidth }, textBoxSizeY{ 9 };
	short textTypedIndexY{ 0 };
	double lastTypeTime{ 0 }, lastSongTime{ 0 };
	bool flipBit{ false }, doType{ false };
	Typewriter typeWriter;
	Ocean ocean;
	int types{ 0 };
public:
	double StartTime() override { return S2_SceneStart; }//535//5.610
	Scene_2_Weather_1(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
};
class Scene_3_Blinky_Credits_Text_Rise : public SceneBase {
	//Ocean ocean;
	CenterBox cb;
	int cbi = 0;
	int last_inputline_subdiv = time->ToBeats(48.1666, 8);
	int last_weather_subdiv = time->ToBeats(48.1666, 8);
	bool blinkIntroActive{ true }, scrollTextActive{ false }, fadeOutActive{ false }, first_box = true, first_text_scroll = true;
	char glitchStage{ 1 }, weatherStage{ 1 }, cheese{ 0 };
	char centerBoxLoops{ 0 };
	unsigned char lastKnownLinesProcessed{ 0 };
	short weatherInterval{ 64 };
	static const int weatherBoxTopLeftX{ nScreenWidth - 29 }, weatherBoxTopLeftY{ 14 };
	int weather_increments{ 0 };
	Weather weather;
	double lastSongTime{ 0 };
	HistoryLog<19> historyLog;
	StaggeredWriter staggeredWriter;
	ColoredString inputLine;
	Distribution<int_dist> glitch_999;

	std::vector<int> sceneEndFadeTracker;
	std::vector<std::pair<int, int>> weatherTextPositions;
public:
	double StartTime() override { return S2_SceneEnd; }
	Scene_3_Blinky_Credits_Text_Rise(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
	void PrintWeather(int id);
	bool BlinkyIntro(unsigned int index, unsigned int start_subdiv, int boxOverride = 0);
	bool ScrollText();
	bool FadeOut();
	void Cheese();
};
class Scene_4_Loading_Bar : public SceneBase {
	static constexpr wchar_t hexAlphabet[] = L"0123456789abcdef";
	std::wstring financial_line;
	Distribution<int_dist> hextet_15;
	unsigned short last_progress_bar_subdiv = time->ToBeats(134.05, 8), //3199
		slow_bar_start = time->ToBeats(134.05, 8), //3199
		text_start = time->ToBeats(134.133798, 8), //3201
		error_bar_start = time->ToBeats(144.1, 8), //3439
		ocean_start = time->ToBeats(144.28333, 8), //3443
		fast_bar_start = time->ToBeats(144.45, 8); //3447
	unsigned short progress_bar{ 0 };
	StaggeredWriter sw;
	Ocean ocean;
public:
	double StartTime() override { return 128.2; }
	Scene_4_Loading_Bar(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
	void SlowLoadingBar();
	void RunawayLoadingBar();
	void HextetErr();
	void FastLoadingBar();
	void DrawLoadingBarBox();
};
class Scene_5_Diagnosis : public SceneBase {

	unsigned char day{ 1 };
	unsigned char date_len{ 0 };
	unsigned char search_timer{ 1 };
	unsigned short last_date_subdiv = 3457, last_search_subdiv = 3457;
	std::wstring diagnosis_line, search_line{ L"" }, date_line{ L"02.02.2019 Unknown" };
	HistoryLog<7> historyLog;
	StaggeredWriter sw1, sw2, sw3;
	std::wstring_view clearstr;
public:
	double StartTime() override { return S4_SceneEnd; }
	Scene_5_Diagnosis(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
};
class Scene_6_Weather_2 : public SceneBase {
	const short textBoxSizeX{ nScreenWidth }, textBoxSizeY{ 9 };
	Ocean ocean;
	Typewriter typeWriter;
	std::vector<std::wstring> glitchCells[3];
public:
	double StartTime() override { return S5_SceneEnd; }
	Scene_6_Weather_2(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
	void PrintText(short glitchSet = -1);
};
class Scene_7_Access_Points : public SceneBase {
	StaggeredWriter sw;
	std::wstring sending_line;
	unsigned short ping{ 0 }, x1{ 0 }, y1{ 0 };
public:
	double StartTime() override { return S6_SceneEnd; }
	Scene_7_Access_Points(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
};
class Scene_8 : public SceneBase {
	StaggeredWriter sw;
	CenterBox cb;
	unsigned short cbt = 0, last_centerbox_subdiv = time->ToBeats(S8_SceneStart, 8u);
	bool first_box = true;
	std::wstring sending_line;
public:
	double StartTime() override { return S7_SceneEnd; }
	Scene_8(SceneConstructorDataWithTypes);
	void Init() override;
	bool Run() override;
};

#endif