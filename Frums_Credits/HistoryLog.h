#ifndef HISTORYLOG_H
#define HISTORYLOG_H
#include "Misc.h"
#include "Console.h"
template <unsigned short textHistorySize>
class HistoryLog {
public:
	static_assert(textHistorySize > 0, "Don't make a 0 size array dingus");
	unsigned short index{ 0 };
	unsigned short GetIndex() const noexcept { return index; }
	const unsigned short size = textHistorySize;
	//First prefix indicates to clear the log
	ColoredString textHistory[textHistorySize]{ {strType{},ConsoleTypes::F_WHITE} };
	HistoryLog() =default;

	void Clear() { for (unsigned short i = index = 0; i < size; i++) textHistory[i] = { strType{}, ConsoleTypes::F_WHITE }; }

	void PushPop(ColoredString&& text_) {
		index = (index + 1) % size;
		textHistory[index] = std::move(text_);
	}
	void PushPop(ColoredString& text_) {
		index = (index + 1) % size;
		textHistory[index] = std::move(text_);
	}

	ColoredString& Peek() { return textHistory[index]; }
	ColoredString& Peek(unsigned short index_) {
		if (index_ >= size) throw "Peek subscript out of bounds";
		return textHistory[index_];
	}
	ColoredString& PeekNext() { return Peek((index + 1) % size); }
	ColoredString& PeekPrev() { return Peek((index + size - 1) % size); }

	unsigned short Increment() { return index = (index + 1) % size; }
	unsigned short Decrement() { return index = (index + size - 1) % size; }
	//historylog++;
	unsigned short operator ++(int) { return Increment(); }
	unsigned short operator --(int) { return Decrement(); }
	//++historylog;
	unsigned short operator ++() { return Increment(); }
	unsigned short operator --() { return Decrement(); }
};

#endif