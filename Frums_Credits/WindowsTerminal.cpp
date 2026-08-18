#include "WindowsTerminal.h"
#include "errhandlingapi.h"
#ifdef USE_WINDOWS_TERMINAL
int WindowsTerminal::CreateConsoleBuffer() {
    SDL_SetMainReady();
#ifdef SDL_WINDOW_CREATED
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
#else
    if (SDL_Init(0) < 0)
#endif
    {    
        std::cerr << "SDL could not Initialize! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }
    SetConsoleTitleA("Frums - Credits");
#ifdef SDL_WINDOW_CREATED
    SDL_Window* SDLwindow = SDL_CreateWindow(
        "Frums - Credits",
        10, 30,
        265, 1,
        SDL_WINDOW_SHOWN
    );
    window = Resource<SDL_Window>(SDLwindow, [](SDL_Window* m) {if (m) SDL_DestroyWindow(m); });
    SDL_SetWindowMaximumSize(SDLwindow, 1000, 1);
    SDL_SetWindowAlwaysOnTop(SDLwindow, SDL_TRUE);
    FillWindow(ConsoleTypes::BLACK);
#endif

    for(size_t i = 0; i < numConsoleBuffers; i++) {
        hConsole[i] = CreateConsoleScreenBuffer(GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        if (hConsole[i] == INVALID_HANDLE_VALUE) {
#pragma warning(disable : 6387)
            SetConsoleActiveScreenBuffer(hConsole[i]);
            std::cout << "\x1b[?25l";
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(hConsole[i], &cursorInfo);
            cursorInfo.bVisible = false; // FALSE to hide, TRUE to show
            SetConsoleCursorInfo(hConsole[i], &cursorInfo);
#pragma warning(default : 6387)
            std::cerr << "CreateConsoleScreenBuffer " << i << " error: " << GetLastError() << '\n';
            for (size_t i = 0; i < numConsoleBuffers; i++) {
                if (hConsole[i] != INVALID_HANDLE_VALUE && hConsole[i] != 0) {
                    CloseHandle(hConsole[i]);
                    hConsole[i] = INVALID_HANDLE_VALUE;
                }
				else hConsole[i] = INVALID_HANDLE_VALUE;
            }
            return EXIT_FAILURE;
            break;
		}
	}

    SetConsoleActiveScreenBuffer(hConsole[0]);



    COORD largest = GetLargestConsoleWindowSize(hConsole[0]); 

    if (nScreenWidth > largest.X || nScreenHeight > largest.Y) {
        std::cerr << "Requested console size (" << nScreenWidth << "x" << nScreenHeight
            << ") is larger than maximum (" << largest.X << "x" << largest.Y << ").\n";
        SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE)); // try restore

        for (auto& h : hConsole) {
            CloseHandle(h);
            h = INVALID_HANDLE_VALUE;
        }

        return EXIT_FAILURE;
    }
    SMALL_RECT tmpRect = { 0, 0, 1, 1 };

    for (auto& h : hConsole) {
        if (!SetConsoleWindowInfo(h, TRUE, &tmpRect)) {
            std::cerr << "Temp SetConsoleWindowInfo error: " << GetLastError() << '\n';
            return EXIT_FAILURE;
        }
	}


    COORD bufferSize = {(SHORT) nScreenWidth, (SHORT) nScreenHeight};

    for (auto& h : hConsole) {
        if (!SetConsoleScreenBufferSize(h, bufferSize)) {
            std::cerr << "SetConsoleScreenBufferSize error: " << GetLastError() << '\n';
            return EXIT_FAILURE;
        }
    }

    SMALL_RECT windowRect = {0, 0, (SHORT) (nScreenWidth - 1), (SHORT) (nScreenHeight - 1)}; 
    for (auto& h : hConsole) {
        if (!SetConsoleWindowInfo(h, TRUE, &windowRect)) {
            std::cerr << "SetConsoleWindowInfo error: " << GetLastError() << '\n';
            return EXIT_FAILURE;
        }
    }
    consoleWnd = GetConsoleWindow();
    if (consoleWnd) {
#ifdef useBufferedConsole
#else
#endif
        LONG style = GetWindowLong(consoleWnd, GWL_STYLE);
        style &= ~WS_SIZEBOX;      // disable resizing by dragging
        style &= ~WS_MAXIMIZEBOX;  // disable maximize button
        SetWindowLong(consoleWnd, GWL_STYLE, style);

        SetWindowPos(consoleWnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
#ifndef NOSCROLL
        ShowScrollBar(consoleWnd, SB_BOTH, FALSE);
#endif
        // Hide the cursor
        CONSOLE_CURSOR_INFO cursorInfo;
        if (GetConsoleCursorInfo(consoleHandle, &cursorInfo)) {
            cursorInfo.dwSize = 1;
            cursorInfo.bVisible = FALSE;
            SetConsoleCursorInfo(consoleHandle, &cursorInfo);
        }

        DWORD outMode = 0;
        if (!GetConsoleMode(consoleHandle, &outMode)) outMode = 0;
        outMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT;
        SetConsoleMode(consoleHandle, outMode);
    }

    //Cell clearCell{L' ', BLACK};
    ClearInternal();
    //std::fill_n(screen, nScreenSize, clearCell);
    //std::fill_n(prevScreen, nScreenSize, clearCell);

    // Restore SDL window / focus
#ifdef SDL_WINDOW_CREATED
    SDL_RaiseWindow(SDLwindow);
    SDL_SetWindowInputFocus(SDLwindow);
#endif
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO info{};
    GetConsoleScreenBufferInfo(h, &info);

    typedef BOOL(WINAPI* SetConsoleHostFn)(DWORD);

    HMODULE hKernel = GetModuleHandleW(L"kernel32.dll");
    if (hKernel) {
        auto SetConsoleHost = (SetConsoleHostFn) GetProcAddress(hKernel, "SetConsoleHost");
        if (SetConsoleHost) {
            // 0 = legacy host
            SetConsoleHost(0);
        }
    }
    else {
        std::cerr << "Failed to Get handle to kernel32.dll\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#ifndef titleChangeAffectsConsoleWindowInsteadOfSdlWindow
int WindowsTerminal::GetTitleTextSize(const std::string& text) {
    const std::wstring str(text.begin(), text.end());

    NONCLIENTMETRICSW ncm = {0};
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);

    HFONT hFont = CreateFontIndirectW(&ncm.lfCaptionFont);

    HDC hdc = GetDC(NULL);
    HFONT old = (HFONT) SelectObject(hdc, hFont);

    SIZE size{};
    GetTextExtentPoint32W(hdc, str.c_str(), (int) str.length(), &size);

    SelectObject(hdc, old);
    ReleaseDC(NULL, hdc);
    DeleteObject(hFont);

    HWND hwnd = GetActiveWindow();
    if (!hwnd)
        return size.cx + 200; // fallback

    UINT dpi = GetDpiForWindow(hwnd);

    auto Scale = [&](int x) { return MulDiv(x, dpi, 96); };
    int border = Scale(GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi)) * 2;
    int iconWidth = Scale(GetSystemMetricsForDpi(SM_CXSMICON, dpi)) + Scale(8);
    int buttonWidth = Scale(GetSystemMetricsForDpi(SM_CXSIZE, dpi)) * 3;
    int padding = Scale(20) + Scale(24);
    return size.cx + border + iconWidth + buttonWidth + padding;
}
#endif
void WindowsTerminal::FillWindow(ConsoleTypes::Color color) {
    // Ensure renderer exists
    if (renderer.Get() == nullptr) {
        SDL_Renderer* renderTest = SDL_GetRenderer(window.Get());
        if (renderTest == nullptr) {
            renderTest = SDL_CreateRenderer(window.Get(), -1, SDL_RENDERER_ACCELERATED);
            if (renderTest == nullptr) return;
        }
        renderer = Resource<SDL_Renderer>(renderTest, [](SDL_Renderer* m) { SDL_DestroyRenderer(m); });
    }

    SDL_SetRenderDrawColor(renderer.Get(), (color & FOREGROUND_RED ? (color & FOREGROUND_INTENSITY ? 255 : 128) : 0),
                           (color & FOREGROUND_GREEN ? (color & FOREGROUND_INTENSITY ? 255 : 128) : 0),
                           (color & FOREGROUND_BLUE ? (color & FOREGROUND_INTENSITY ? 255 : 128) : 0), 255);
    SDL_RenderClear(renderer.Get());
    SDL_RenderPresent(renderer.Get());
}
void WindowsTerminal::Write(intType rowX, intType colY, charType ch, ConsoleTypes::Color c) {
    if (c == 0) c = ConsoleTypes::F_WHITE;
    if (colY < 0 || colY >= nScreenHeight || rowX < 0 || rowX >= nScreenWidth) return;

    screenData[colY * nScreenWidth + rowX] = { ch ,c };

	//screenData[colY * nScreenWidth + rowX].Attributes = c;
    //screen[colY * nScreenWidth + rowX] = {ch, c};
	//screen[colY * nScreenWidth + rowX].attr = c;
	//screen[colY * nScreenWidth + rowX].ch = ch;
}

void WindowsTerminal::Write(intType rowX, intType colY, const strType s, ConsoleTypes::Color c) {
    if (s.empty()) return;
    if (s.size() == 1) { screenData[colY * nScreenWidth + rowX] = {s[0], c}; return; }
    if (c == 0) c = ConsoleTypes::F_WHITE;
    for (size_t i = 0; i < s.size() && rowX + (int)i < nScreenWidth; ++i) {
		screenData[static_cast<unsigned long long>(colY) * nScreenWidth + rowX + i] = { s[i], c };
    }
}

void WindowsTerminal::ClearInternal() {
    std::fill_n(screenData, nScreenSize, CHAR_INFO{ clearCell.ch, clearCell.attr });
}

int WindowsTerminal::DisplayFrame() {
    prints++;
#ifdef useWideCharacters
    if (!WriteConsoleOutputW(consoleHandle, screenData, bufferSize, {0,0}, &writeRegion))
#else
    if (!WriteConsoleOutputA(consoleHandle, screenData, bufferSize, {0,0}, &writeRegion))
#endif
    {DBOUT(std::string("WriteConsoleOutput error: ") + std::to_string(HRESULT_FROM_WIN32(GetLastError())));}
    //DBOUT(std::to_string(currentConsoleHandleIdx));
    if (!SetConsoleActiveScreenBuffer(consoleHandle)) {
        std::cerr << "SetConsoleActiveScreenBuffer error: " << GetLastError() << '\n';
		return EXIT_FAILURE;
    }   
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole[currentConsoleHandleIdx], &cursorInfo);
    cursorInfo.bVisible = false; // FALSE to hide, TRUE to show
    SetConsoleCursorInfo(hConsole[currentConsoleHandleIdx], &cursorInfo);
	currentConsoleHandleIdx = nextConsoleHandleIdx;
    return EXIT_SUCCESS;
}

void WindowsTerminal::ChangeConsoleTitle(const std::wstring& title) {
#ifndef SHOWCASE_MODE
#if defined titleChangeAffectsConsoleWindowInsteadOfSdlWindow or not defined SDL_WINDOW_CREATED
#ifdef useWideCharacters
    SetConsoleTitleW(title.c_str());
#else
    SetConsoleTitleA(title.c_str());
#endif
    return;
#else
    std::string str = ConvertWideToANSI(title);
    SDL_SetWindowTitle(window.Get(), str.c_str());
    SDL_SetWindowSize(window.Get(), GetTitleTextSize(str), 1);
    //if (windowWidth < newWidth) {
    //    windowWidth = newWidth;
    //    FillWindow();
    //}
#endif
#else
#ifdef useWideCharacters
    SetConsoleTitleW(title.c_str());
#else
    SetConsoleTitleA(title.c_str());
#endif
#endif
}
#endif