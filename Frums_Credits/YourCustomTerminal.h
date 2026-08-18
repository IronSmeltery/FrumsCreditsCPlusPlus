#ifndef YOURCUSTOMTERMINAL_H
#define YOURCUSTOMTERMINAL_H
#include "Console.h"
struct YourCustomTerminal : Console {
    YourCustomTerminal(int& result) {}
    void Write(intType rowX, intType colY, charType ch, ConsoleTypes::Color c = ConsoleTypes::F_WHITE) override {}
    void Write(intType rowX, intType colY, const strType s, ConsoleTypes::Color c = ConsoleTypes::F_WHITE) override {}
    void ChangeConsoleTitle(const std::wstring& title) override {}
    int DisplayFrame() override { return 0; }
protected:
    void ClearInternal() override {}
};
#endif