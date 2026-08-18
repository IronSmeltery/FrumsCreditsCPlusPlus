#ifndef TYPEWRITER
#define TYPEWRITER
#include "Misc.h"
#include "ConsoleTypes.h"
#include "GlobalRNGDistributions.h"


class Typewriter {
    const wchar_t glitchChars[13] = L"...  `=/?-$%";
    std::vector<std::wstring> sourceText, returnText;
    Distribution<int_dist> glitch_999, glitchChar_12;
    float glitchPercent{0.0f};
public:
    int currColTyping{0};
    short currRowTyping{0};
    short skipsLeft{0};
    bool finished{false};
    Typewriter() = default;
    Typewriter(SRef<Randomizer> randomizer_);
    void SetGlitchPercentage(float percent) { glitchPercent = Clamp(percent); }
    void SetText(std::vector<std::wstring> text_);
    void Reset();
    void Type();
    std::vector<std::wstring> GetText();
};

#endif
