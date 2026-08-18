#ifndef CONSOLE
#define CONSOLE
#include "Misc.h"
#include "ResourceHandler.h"
#include "Defines.h"
#include "ConsoleTypes.h"



#ifndef CONSOLE_CLEAR_CELL_COLOR
#define CONSOLE_CLEAR_CELL_COLOR ConsoleTypes::F_WHITE
#endif

#ifdef useWideCharacters
static constexpr ConsoleTypes::Cell clearCell(L' ', CONSOLE_CLEAR_CELL_COLOR);
static const std::wstring clearStr(nScreenWidth, L' ');
#else
static constexpr ConsoleTypes::Cell clearCell(' ', CONSOLE_CLEAR_CELL_COLOR);
static const std::string clearStr(nScreenWidth, ' ');
#endif
struct Console {
    unsigned long long prints{ 0 };
	//-----{Required overrides for any terminal implementation}------------------
    virtual void Write(intType rowX, intType colY, charType ch, ConsoleTypes::Color c) = 0;
    virtual void Write(intType rowX, intType colY, const strType s, ConsoleTypes::Color c) = 0;
    virtual int DisplayFrame() = 0;    
protected:
    virtual void ClearInternal() = 0;
public://----{Optional overrides}------------------------------------------------
    virtual void ChangeConsoleTitle(const std::wstring& title) {};
    //---------------------------------------------------------------------------

    virtual int Clear() final { ClearInternal(); return DisplayFrame(); }
    inline intType ClearRect(intType startX, intType startY, intType sizeX, intType sizeY, bool noUpdate = false, ConsoleTypes::Color color = CONSOLE_CLEAR_CELL_COLOR) {
        if (startX == startY == 0 && (sizeX == nScreenWidth || sizeX == nScreenWidth - 1) && (sizeY == nScreenHeight || sizeY == nScreenHeight - 1)) { return Clear(); }
		RectCheck(startX, startY, sizeX, sizeY);
        strType clearStr(sizeX, ' ');
        for (intType i = startY; i < startY + sizeY; i++) Write(startX, i, clearStr, color);
        return noUpdate ? false : DisplayFrame();
    }
    virtual ~Console() = default;
#ifdef enableWideStringConversion
    static std::string ConvertWideToANSI(const std::wstring& wstr) {
        int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
        std::string str(count, 0);
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
        return str;
    }
    static std::wstring ConvertANSIToWide(const std::string& str) {
        int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), NULL, 0);
        std::wstring wstr(count, 0);
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], count);
        return wstr;
	}
public:
#ifdef useWideCharacters
    void Write(intType rowX, intType colY, std::string s, ConsoleTypes::Color c) {Write(rowX, colY, std::wstring(s.begin(), s.end()), c);};
    void Write(intType rowX, intType colY, const char ch, ConsoleTypes::Color c) {Write(rowX, colY, static_cast<wchar_t>(ch), c);};
#else
    void Write(intType rowX, intType colY, std::wstring s, ConsoleTypes::Color c) {Write(rowX, colY, ConvertWideToANSI(s), c);};
	void Write(intType rowX, intType colY, const wchar_t ch, ConsoleTypes::Color c) {Write(rowX, colY, static_cast<char>(ch), c); };
#endif
#endif
    static void constexpr RectCheck(intType startX, intType startY, intType sizeX, intType sizeY) {
        if (startX < 0) throw std::exception("startX must be positive");
        if (startY < 0) throw std::exception("startY must be positive");
        if (sizeX <= 0) throw std::exception("sizeX must be greater than 0");
        if (sizeY <= 0) throw std::exception("sizeY must be greater than 0");
        if (sizeX > nScreenWidth) throw std::exception("sizeX is larger than screen buffer");
        if (sizeY > nScreenHeight) throw std::exception("sizeY is larger than screen buffer");
        if (startX + sizeX > nScreenWidth) throw std::exception("startX + sizeX is beyond screen buffer");
        if (startY + sizeY > nScreenHeight) throw std::exception("startY + sizeY is beyond screen buffer");
    }
};

#ifndef DBOUT_DEFINED
#define DBOUT_DEFINED
//Prints to the visual studio debug output, only works when running in the debugger.
static void DBOUT(std::string s) {OutputDebugStringA((s + '\n').c_str());}
static void DBOUT(std::wstring s) {OutputDebugStringW((s + L'\n').c_str());}
#endif

#ifdef USE_WINDOWS_TERMINAL
#include "WindowsTerminal.h"
#elif defined USE_YOURCUSTOMTERMINAL
#include "YourCustomTerminal.h"
#else
#error "No terminal defined.Please define either WINDOWSTERMINAL or YOURCUSTOMTERMINAL."
#define NO_TERMINAL_DEFINED
#endif
#if defined USE_WINDOWS_TERMINAL and defined USE_YOURCUSTOMTERMINAL
#error "Multple terminals defined, limit of one allowed."
#endif
#endif