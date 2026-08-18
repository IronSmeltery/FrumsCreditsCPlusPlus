#ifndef CONSOLETYPES
#define CONSOLETYPES
//#include "consoleapi2.h"
static constexpr int nScreenWidth = 80;   // Console width
static constexpr int nScreenHeight = 24;  // Console height
static constexpr int nScreenSize = nScreenWidth * nScreenHeight; // Total console memory size
namespace ConsoleTypes {
#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.
    enum Color : unsigned short {
        BLACK = 0x0000,
        // Basic foreground colors
        F_GRAY = FOREGROUND_INTENSITY,
        F_RED = FOREGROUND_RED,
        F_RED_BRIGHT = F_RED | F_GRAY,
        F_GREEN = FOREGROUND_GREEN,
        F_GREEN_BRIGHT = F_GREEN | F_GRAY,
        F_BLUE = FOREGROUND_BLUE,
        F_BLUE_BRIGHT = F_BLUE | F_GRAY,
        // Combinations
        F_CYAN = F_GREEN | F_BLUE,
        F_CYAN_BRIGHT = F_GREEN | F_BLUE | F_GRAY,
        F_MAGENTA = F_RED | F_BLUE,
        F_MAGENTA_BRIGHT = F_RED | F_BLUE | F_GRAY,
        F_YELLOW = F_RED | F_GREEN,
        F_YELLOW_BRIGHT = F_RED | F_GREEN | F_GRAY,
        F_WHITE = F_RED | F_GREEN | F_BLUE,
        F_WHITE_BRIGHT = F_RED | F_GREEN | F_BLUE | F_GRAY,

        // Basic background colors
        B_GRAY = BACKGROUND_INTENSITY,
        B_RED = BACKGROUND_RED,
        B_RED_BRIGHT = B_RED | B_GRAY,
        B_GREEN = BACKGROUND_GREEN,
        B_GREEN_BRIGHT = B_GREEN | B_GRAY,
        B_BLUE = BACKGROUND_BLUE,
        B_BLUE_BRIGHT = B_BLUE | B_GRAY,
        // Combinations
        B_CYAN = B_GREEN | B_BLUE,
        B_CYAN_BRIGHT = B_GREEN | B_BLUE | B_GRAY,
        B_MAGENTA = B_RED | B_BLUE,
        B_MAGENTA_BRIGHT = B_RED | B_BLUE | B_GRAY,
        B_YELLOW = B_RED | B_GREEN,
        B_YELLOW_BRIGHT = B_RED | B_GREEN | B_GRAY,
        B_WHITE = B_RED | B_GREEN | B_BLUE,
        B_WHITE_BRIGHT = B_RED | B_GREEN | B_BLUE | B_GRAY

    };
    struct Cell {
        charType ch{' '};
        WORD attr { Color::F_WHITE };
        constexpr Cell() = default;
        constexpr Cell(charType ch_, ConsoleTypes::Color c):ch(ch_),attr(c){}
        bool operator==(const Cell& other) const { return ch == other.ch && attr == other.attr; }
        bool operator!=(const Cell& other) const { return !(*this == other); }
    };
};
#endif