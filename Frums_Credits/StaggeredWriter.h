#ifndef STAGGEREDWRITER_H
#define STAGGEREDWRITER_H
#include "Misc.h"
#include "Distributions.h"
#include "GlobalRNGDistributions.h"
class StaggeredWriter {
	const char glitchChars[13] = "...  `=/?-$%";
public:
	short wait_timer{ 0 };
	int linesProcessed{ -1 };
	char end_of_line{0};
	bool queue_finished{ false };
private:
	short currRowNextCharIndex{ 0 };
	float glitchPercent{ 0.0f };
	std::queue<std::wstring> textQueue;
	std::wstring currentText{}, returnText{}, prevReturnText{};
	Distribution<int_dist> glitch_999, glitchChar_12;

public:
	StaggeredWriter() = default;
	StaggeredWriter(SRef<Randomizer> randomizer_);
	void SetGlitchPercentage(float percent) { return; glitchPercent = Clamp(percent); }
	void SetText(std::queue<std::wstring>& text_);
	void SetText(std::vector<std::wstring>& text_);
	void Type();
	std::wstring GetText();
	void ResetLine();
};
#endif