#include <typeinfo>
#include <iostream>
#include "FrameTimer.h"
#include "Misc.h"
#include <numeric>

FrameTimer::FrameTimer() : startTime(0), currentTime(0){}

FrameTimer::~FrameTimer() {}

void FrameTimer::Start() {
	startTime = SDL_GetTicks();
	currentTime = SDL_GetTicks();
}

void FrameTimer::UpdateFrameTicks() {
	//if (!wasPaused && paused) {
	//	wasPaused = true;
	//	return;
	//}
	//else if (wasPaused && !paused) {
	//	wasPaused = false;
	//	startTime += SDL_GetTicks() - currentTime; // Adjust start time to account for pause duration
	//	return;
	//}
	currentTime = SDL_GetTicks();
	++frameCount;
	if (currentTime > startTime + 1000) {
		fps = frameCount / ((currentTime - startTime) / 1000.0);
		frameCount = 0;
		startTime = currentTime;
		// Update FPS samples for average calculation
		std::move(fpsSamples.begin() + 1, fpsSamples.end(), fpsSamples.begin());
		fpsSamples.back() = fps;
 		averageFPS = std::accumulate(fpsSamples.begin(), fpsSamples.end(), 0.0) / sampleCount;
		if (!avgReady && std::count(fpsSamples.begin(), fpsSamples.end(), 0.0) == 0) {
			avgReady = true;
		}
	}
}

//float Timer::GetDeltaTime() const {
//	return static_cast<float>(currentTime - prevTicks) / 1000.0f;
//}
//
//unsigned int Timer::GetSleepTime(const unsigned int fps_) const {
//	unsigned int milliSecsPerFrame = 1000 / fps_;
//	if (milliSecsPerFrame == 0) {
//		return 0;
//	}
//	unsigned int sleepTime = milliSecsPerFrame - SDL_GetTicks();
//	if (sleepTime > milliSecsPerFrame) {
//		return milliSecsPerFrame;
//	}
//	return sleepTime;
//}
//
//float Timer::GetCurrentTicks() const {
//	return static_cast<float>(currentTime) / 1000.0f;
//}

double FrameTimer::GetFPS() const {return fps;}
double FrameTimer::GetAverageFPS() const { return avgReady ? averageFPS : std::count(fpsSamples.begin(), fpsSamples.end(), 0.0); }
