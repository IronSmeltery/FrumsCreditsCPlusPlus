#ifndef SCENE_DATA
#define SCENE_DATA
#include <utility>

#define S1_SongStart 0.0
#define S1_SceneStart 2.9
#define S1_SceneEnd 5.2666666667
#define S2_SceneStart 5.3333333
#define S2_SecondTextSet 15.2
#define S2_ThirdTextSet 29.5
#define S2_FourthTextSet 44.5
#define S2_SceneEnd 47.5
#define S3_SceneStart 48.08333333
#define S3_BoxEnd 77.4301676
#define S3_SecondTextSet 101.4
#define S3_WeatherFadeOut 123.0
#define S3_SceneEnd 128.2
#define S4_SceneStart 134.0
#define S4_StartOfBar 134.0
#define S4_FastPart 143.5
#define S4_SceneEnd 144.785
#define S5_SceneStart 144.8666667
#define S5_PBS 163.3833333
#define S5_SceneEnd 166.0
#define S6_SceneStart 166.0666667
#define S6_Elsewhere 186.7833333
#define S6_SceneEnd 187.7333333
#define S7_SceneStart 189.7
#define S7_StartOfWords 209.1666667
#define S7_PBS14 227.9166667
#define S7_SceneEnd 233.2666667
#define S8_SceneStart 233.2833333
#define S8_StartOfWords 245.4166667
#define S8_NearEnd 265.3333333
#define S8_SceneEnd 277.9833333
#define S8_SongEnd 277.9833333
constexpr std::pair<int, double> skipTime[]{
	/*00*/ {1,S1_SongStart}, //start of song (scene 1)
	/*01*/ {2,S2_SceneStart}, //start of scene 2
	/*02*/ {2,S2_SecondTextSet},//scene 2 second text set (tonight)
	/*03*/ {2,S2_ThirdTextSet},//scene 2 third text set (friday)
	/*04*/ {2,S2_FourthTextSet},//scene 2 fourth text set (saturday)
	/*05*/ {3,S3_SceneStart},//start of scene 3
	/*06*/ {3,S3_BoxEnd},//scene 3 box end (start of first text set (funding))
	/*07*/ {3,S3_SecondTextSet},//scene 3 start of second text set (corporation)
	/*08*/ {3,S3_WeatherFadeOut},//scene 3 start of weather fade out
	/*09*/ {3,S3_SceneEnd},//scene 3 end (start of blank screen)
	/*10*/ {4,S4_StartOfBar},//scene 4 start (start of slow loading bar)
	/*11*/ {4,S4_FastPart},//scene 4 fast part (error and fast loading bar)
	/*12*/ {5,S5_SceneStart},//start of scene 5
	/*13*/ {5,S5_PBS},//scene 5 start of "this is pbs"
	/*14*/ {6,S6_SceneStart},//start of scene 6
	/*15*/ {6,S6_Elsewhere},//scene 6 start of "elsewhere"
	/*16*/ {6,S6_SceneEnd},//scene 6 end (start of blank screen)
	/*17*/ {7,S7_SceneStart},//start of scene 7 (start of access points fade in)
	/*18*/ {7,S7_StartOfWords},//scene 7 start of lyrics (funding)
	/*19*/ {7,S7_PBS14},//scene 7 start of pbs #14 being pinged
	/*20*/ {8,S8_SceneStart},//start of scene 8
	/*21*/ {8,S8_StartOfWords},//scene 8 start of lyrics (funding)
	/*22*/ {8,S8_NearEnd},//scene 8 start of lyrics (funding)
	/*23*/ {8,S8_SceneEnd},//scene 8 end (end of song)
};

#endif