#include "Typewriter.h"

Typewriter::Typewriter(SRef<Randomizer> randomizer_) {
    if (!randomizer_.get()) return;
    glitch_999 = Distribution<int_dist>(*randomizer_, 0, 999);
    glitchChar_12 = Distribution<int_dist>(*randomizer_, 0, sizeof(glitchChars) - 1);
}

void Typewriter::SetText(std::vector<std::wstring> text_) {
    sourceText = text_;
    for (auto& str : sourceText)
        str.erase(std::remove(str.begin(), str.end(), L'\n'), str.end());
    Reset();
}

void Typewriter::Reset() {
    returnText.clear();
    finished = false;
    currRowTyping = 0;
    currColTyping = 0;
}

// Type 'num' characters or beats
void Typewriter::Type() {
    if (finished || sourceText.empty()) { return; }
    if (skipsLeft > 0) {
        skipsLeft--;
        return;
    }

    // End of all text
    if (currRowTyping >= sourceText.size()) {
        finished = true;
        return;
    }
#define rowText (sourceText)[currRowTyping]

    //std::wstring& rowText = (sourceText)[currRowTyping];


    while (returnText.size() <= currRowTyping) returnText.push_back({});

    wchar_t ch = rowText[currColTyping];

    switch (ch) {
    case L'~': {
        size_t tilde_start = rowText.find(L'~', currColTyping);
		size_t tilde_end = rowText.find_first_not_of(L'~', tilde_start);
        skipsLeft = (short)(tilde_end - tilde_start - 1);
        currColTyping = (int)tilde_end;
        break;
    } 
    case L'@': {
        // Type next 3 characters immediately

		std::wstring append = rowText.substr(currColTyping+1, 3);
        returnText[currRowTyping] += append;
        currColTyping += 4;
        if (currColTyping >= rowText.length()) {
            currColTyping = 0;
            currRowTyping++;
        }
        break;

    }
    case L'!': __debugbreak(); break;
    default: {
        returnText[currRowTyping] += ch;
        currColTyping++;
        break;
    }
    }

	// End of current row: move to next
	if (currColTyping >= rowText.size()) {
		currRowTyping++;
		currColTyping = 0;
	}
	bool newline = false;
	// Ensure returnText has a row for current source row
	while (returnText.size() <= currRowTyping) {
		newline = true;
		returnText.push_back({});
		if (currRowTyping < sourceText.size() and rowText.empty()) currRowTyping++;
		if (currRowTyping >= sourceText.size()) {
			finished = true;
			return;
		}
	}
	if (newline) { returnText[currRowTyping].reserve(sourceText[currRowTyping].length()); }

}

// Returns the text with optional glitch
std::vector<std::wstring> Typewriter::GetText() {
    if (returnText.empty()) return std::vector<std::wstring> {{L"_"}};
    std::vector<std::wstring> result = returnText;

    // Iterate through all rows
    for (size_t i = 0; i < result.size(); i++) {
        if (!result[i].empty()) {
            if (i == result.size() - 1) result[i] += L'_';
            else if (result[i].back() != ' ') result[i] += L' ';
            std::transform(result[i].begin(), result[i].end(), result[i].begin(),
                            [this](char c) {return glitch_999.Chance(glitchPercent) ? glitchChars[glitchChar_12.Rand()] : c;});
            
        }
    }

    return result;
}
