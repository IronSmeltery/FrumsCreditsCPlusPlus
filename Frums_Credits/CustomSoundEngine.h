#ifndef CUSTOMSOUNDENGINE_H
#define CUSTOMSOUNDENGINE_H
#include "SoundEngine.h"
struct CustomSoundEngine : SoundEngine {
	CustomSoundEngine() {}
	bool PlaySound(const std::string filePath) override { return false; };
	void StopSound() override {};
	void Pause() override {};
	void Resume() override {};
	bool IsPlaying() const override { return false; };
	bool IsPaused() const override { return false; };
	double GetSongTime() override { return 0.0; };
	bool SeekSeconds(double seconds) override { return false; };
	bool SeekBeat(int beatIndex, double secondsPerBeat) override { return false; };
};
#endif