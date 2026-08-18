#ifndef WINDOWSTERMINAL_H
#define WINDOWSTERMINAL_H
#include "Console.h"
#ifndef DISABLE_INPUT
#define SDL_WINDOW_CREATED
#endif
/// <summary>
/// Sets up a console that uses the Windows Terminal's console buffers to display text.
/// Also creates a small SDL window to receive input events unless DISABLE_INPUT is defined.
/// </summary><remarks>
/// This doesn't benefit from more than 2 buffers, 
/// but the defines are set up to allow for more than 2 if you want to experiment with it.
/// Single buffering is supported, but it will be slower and prone to flickering.
/// </remarks>
struct WindowsTerminal : Console {
private:
#ifndef numConsoleBuffers
#define numConsoleBuffers 2
#elif defined numConsoleBuffers and numConsoleBuffers < 1
#error "numConsoleBuffers must be greater than 0"
#endif
    HANDLE hConsole[numConsoleBuffers];
	size_t currentConsoleHandleIdx{ 0 };
    //current is the buffer we're writing stuff to
    //next is the next one to be written to after current is done
    //prev is the one currently being displayed in the console window
    //with 2 buffers, next == prev
#if numConsoleBuffers >= 3
#define nextConsoleHandleIdx ((currentConsoleHandleIdx + 1) % numConsoleBuffers)
#define prevConsoleHandleIdx ((currentConsoleHandleIdx + numConsoleBuffers - 1) % numConsoleBuffers)
#elif numConsoleBuffers == 2
#define nextConsoleHandleIdx !currentConsoleHandleIdx
#define prevConsoleHandleIdx !currentConsoleHandleIdx
#elif numConsoleBuffers == 1
#define nextConsoleHandleIdx 0
#define prevConsoleHandleIdx 0
#endif
#define consoleHandle hConsole[currentConsoleHandleIdx]
#define nextConsoleHandle hConsole[nextConsoleHandleIdx]
#define prevConsoleHandle hConsole[prevConsoleHandleIdx]

#if not defined SHOWCASE_MODE and defined SDL_WINDOW_CREATED and not defined titleChangeAffectsConsoleWindowInsteadOfSdlWindow
    unsigned int windowWidth{ 0 };
#endif
	const COORD bufferSize{ SHORT(nScreenWidth), SHORT(nScreenHeight) };
    SMALL_RECT writeRegion{ 0, 0, nScreenWidth-1, nScreenHeight-1 };
	CHAR_INFO screenData[nScreenSize]{};
    HWND consoleWnd{ nullptr };
    //------
    Resource<SDL_Window> window;
    Resource<SDL_Renderer> renderer;
    int CreateConsoleBuffer();
#ifndef titleChangeAffectsConsoleWindowInsteadOfSdlWindow
    int GetTitleTextSize(const std::string& text);
#endif
    void FillWindow(ConsoleTypes::Color color = ConsoleTypes::F_WHITE_BRIGHT);
public:
    WindowsTerminal(int& result) { result = CreateConsoleBuffer(); }
    void Write(intType rowX, intType colY, charType ch, ConsoleTypes::Color c = ConsoleTypes::F_WHITE) override;
    void Write(intType rowX, intType colY, const strType s, ConsoleTypes::Color c = ConsoleTypes::F_WHITE) override;
    int DisplayFrame() override;
    void ChangeConsoleTitle(const std::wstring& title) override;
protected:
    void ClearInternal() override;
private:
};
#endif