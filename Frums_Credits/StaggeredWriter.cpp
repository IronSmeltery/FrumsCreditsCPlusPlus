#include "StaggeredWriter.h"

StaggeredWriter::StaggeredWriter(SRef<Randomizer> randomizer_) {
	if (!randomizer_.get()) return;
	glitch_999 = Distribution<int_dist>(*randomizer_, 0, 999);
	glitchChar_12 = Distribution<int_dist>(*randomizer_, 0, (int)(sizeof(glitchChars) / sizeof(char) - 1));
}

void StaggeredWriter::SetText(std::queue<std::wstring>& text_) {
	textQueue = text_;
	queue_finished = false;
	linesProcessed = 0;
	ResetLine();
}

void StaggeredWriter::SetText(std::vector<std::wstring>& text_) {
	textQueue = std::queue<std::wstring>{};
	for (auto& str : text_) textQueue.push(str);
	queue_finished = false;
	linesProcessed = 0;
	ResetLine();
}

void StaggeredWriter::Type() {
	if (wait_timer) {
		wait_timer--;
		return;
	}
	if (queue_finished) return;

	if (end_of_line) {
		//if queue is empty, signal that and return
		//else reset_line()
		//increment linesprocessed
		//set wait_timer = 1
		if (textQueue.empty()) {
			queue_finished = true;
			return;
		}
		ResetLine();
		if (currentText[0] == L'@') {
			//parse glitch percent
			float gpc;
			try {
				gpc = std::stof(currentText.substr(1));
			}
			catch (...) { __debugbreak(); }
			SetGlitchPercentage(Percent(gpc, 0.0f, 1000.0f));
			ResetLine();
		}
		linesProcessed++;
		wait_timer = 0;
		return;
	}

	//else...

	/*	
	if line is in progress
		find hash_index
		find append_string
		set returntext += append_string
		find next currindex
		set wait_timer to number of "#"s between printable chars
	*/
	
	if (returnText == L"  ") { returnText.clear(); }

	size_t hash_start = currentText.find(L"#", currRowNextCharIndex);
	size_t hash_end = currentText.find_first_not_of(L"#", hash_start) - 1;
	int print_length = (int) hash_start - currRowNextCharIndex;
	std::wstring append = currentText.substr(currRowNextCharIndex, print_length);
	//if print_length is negative, append is the entire rest of the string
	//this is fine because if there are no more #s we just want to get the full string anyways
	if (glitchPercent > 0) {
		std::transform(append.begin(), append.end(), append.begin(),
			[this](charType c) {return isalpha(c) ? glitch_999.Chance(glitchPercent) ? glitchChars[glitchChar_12.Rand()] : c : c; });
	}
	returnText += append;

	if (hash_start >= currentText.size())    { wait_timer = 0; } //there are no more #s
	else if (hash_end >= currentText.size()) { wait_timer = (short)(currentText.size() - hash_start); } //the rest of the string is #s
	else if (hash_start == hash_end)		 { wait_timer = 0; } //exactly one #
	else 									 { wait_timer = (short)(hash_end - hash_start); }

	if (hash_end >= currentText.size()) {
		end_of_line = true;
		currRowNextCharIndex = (short)currentText.size();
	}
	else { currRowNextCharIndex = (short)(hash_end + 1); }
}

strType StaggeredWriter::GetText() {
	return returnText;
}

void StaggeredWriter::ResetLine() {
	returnText.clear();
	currentText.clear();
	currentText = std::move(textQueue.front());
	textQueue.pop();
	returnText.reserve(currentText.size());
	returnText = L"  ";
	currRowNextCharIndex = 0;
	end_of_line = false;
}
