#ifndef DEFINES_H
#define DEFINES_H
//--------------------------------------------------------------------------------
//#define SHOWCASE_MODE //-{toggle}
//#define DISABLE_INPUT //-{toggle}
#define USE_WINDOWS_TERMINAL //-{toggle}
//#define USE_YOURCUSTOMTERMINAL //-{toggle}
#define MINIAUDIO //-{toggle}
//define CUSTOMSOUNDENGINE //-{toggle}
#define titleChangeAffectsConsoleWindowInsteadOfSdlWindow //-{toggle}
#define useWideCharacters //-{toggle}
#define enableWideStringConversion //-{toggle}
#define numConsoleBuffers 2 //-{must be 1 or greater}
#define globalVolume 0.3125f //-{0.0f to 1.0f}
#define preDecodeAudio //-{toggle}
//#define test_for_debug_removals //-{debugging}
//#define CONSOLE_CLEAR_CELL_COLOR ConsoleTypes::B_GRAY //-{debugging}
//--------------------------------------------------------------------------------
#if numConsoleBuffers >= 2
#define useBufferedConsole
#endif

#ifdef useWideCharacters
constexpr bool ISWIDE = true;
    using charType = wchar_t;
	using strType = std::wstring;
	using strViewType = std::wstring_view;
#else
constexpr bool ISWIDE = false;
    using charType = char;
	using strType = std::string;
	using strViewType = std::string_view;
#endif
	using intType = unsigned int;
#ifdef SHOWCASE_MODE
#undef preDecodeAudio
#undef CONSOLE_CLEAR_CELL_COLOR
#undef test_for_debug_removals
#define DISABLE_INPUT
#endif
#endif
