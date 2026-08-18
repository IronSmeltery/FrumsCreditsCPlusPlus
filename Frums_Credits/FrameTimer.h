#ifndef TIMER_H
#define TIMER_H
#include <array>

class FrameTimer {
private:	
	unsigned int startTime;
	unsigned int currentTime;
	unsigned int frameCount{0};
	double fps{0.0}, averageFPS{0.0};
	static const short sampleCount{5};
	std::array<double, sampleCount> fpsSamples{};
	bool avgReady{false};
public:
	FrameTimer();
	~FrameTimer();

	FrameTimer(const FrameTimer&) = delete;
	FrameTimer(FrameTimer&&) = delete;
	FrameTimer& operator=(const FrameTimer&) = delete;
	FrameTimer& operator=(FrameTimer&&) = delete;

	void Start();
	void UpdateFrameTicks();
	//float GetDeltaTime() const;//remove?
	//unsigned int GetSleepTime(const unsigned int fps_) const;//remove?
	//float GetCurrentTicks() const;//remove?
	double GetFPS() const;
	double GetAverageFPS() const;
};
#endif
