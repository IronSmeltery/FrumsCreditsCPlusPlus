#ifndef CONSOLE_EXTENSIONS_H
#define CONSOLE_EXTENSIONS_H
#include "Console.h"
#include "Defines.h"
struct ConsoleExtensions {
    static void WriteBox(Console& console, intType rowX, intType colY, intType sizeX, intType sizeY, const charType s, ConsoleTypes::Color c) {
        Console::RectCheck(rowX, colY, sizeX, sizeY);
        strType boxStr(sizeX, s);
        for (intType i = colY; i < colY + sizeY; i++) console.Write(rowX, i, boxStr, c);
    }

#ifdef useWideCharacters
    static void WriteBox(Console& console, intType rowX, intType colY, const char ch, ConsoleTypes::Color c) { WriteBox(console, rowX, colY, static_cast<wchar_t>(ch), c); }
#else
    static void WriteBox(Console& console, intType rowX, intType colY, const wchar_t ch, ConsoleTypes::Color c) { WriteBox(console, rowX, colY, static_cast<char>(ch), c); }
#endif

    static intType ClampX(intType x) { return x > nScreenWidth - 1 ? nScreenWidth - 1 : (x < 0 ? 0 : x); }
    static intType ClampY(intType y) { return y > nScreenHeight - 1 ? nScreenHeight - 1 : (y < 0 ? 0 : y); }
	static intType ClampSiseX(intType x, intType sizeX) { return x + sizeX > nScreenWidth ? nScreenWidth - x : sizeX; }
	static intType ClampSiseY(intType y, intType sizeY) { return y + sizeY > nScreenHeight ? nScreenHeight - y : sizeY; }

    struct CellRect {
        intType startX, startY, sizeX, sizeY;
        CellRect(intType x, intType y, intType w, intType h) : startX(x), startY(y), sizeX(w), sizeY(h) {}
        constexpr intType operator[] (int index) const {
            switch (index) {
                case 0: return startX;
                case 1: return startY;
                case 2: return sizeX;
                case 3: return sizeY;
                default: throw std::out_of_range("CellRect index out of range");
            }
		}
    };
    template <typename T = intType>
    static void printDBText(Console& console, std::vector<T> data, intType x = 0, intType y = 0) {
#ifndef SHOWCASE_MODE
        console.Write(x,y, std::accumulate(data.begin(), data.end(), std::wstring{}, [](const std::wstring& a, const T& b) {
            return a + (a.length() > 0 ? L", " : L"") + to_strType(b);
            }), ConsoleTypes::F_WHITE_BRIGHT);
#endif
    }
    static void WriteWrap(Console& console, intType rowX, intType colY, const strType s, ConsoleTypes::Color c) {
		if (nScreenWidth * colY + rowX + s.length() > nScreenSize) throw std::out_of_range("WriteWrap: string exceeds console buffer size");
        if (s.length() + colY < nScreenWidth) {
            console.Write(rowX, colY, s, c);
            return;
        }
        intType i = 0;
        while (i < s.length()) {
            console.Write(rowX,colY,s.substr(i, nScreenWidth - rowX),c);
            rowX = 0;
            colY++;
            i += nScreenWidth - rowX;
        }
    }

};
#endif