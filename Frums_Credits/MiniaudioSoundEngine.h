#ifndef MINIAUDIOSOUNDENGINE_H
#define MINIAUDIOSOUNDENGINE_H
#include "SoundEngine.h"
#pragma warning (push)
#pragma warning(disable: 6385 6386 6011)
#include "miniaudio.h"
#pragma warning (pop)
#include "ResourceHandler.h"
class MiniaudioSoundEngine : public SoundEngine {
public:
	MiniaudioSoundEngine(int& result);
	~MiniaudioSoundEngine();
	void EngineLoop() override;
	bool PlaySound(const std::string filePath) override;
	void StopSound() override;
	void Pause() override;
	void Resume() override;
	bool IsPlaying() const override;
	bool IsPaused() const override;
	double GetSongTime() override;
	bool SeekSeconds(double seconds) override;
	bool SeekBeat(int beatIndex, double secondsPerBeat) override;
	bool SetVolume(float volume) override;
	float GetVolume() override;
private:
	ma_engine engine{};
	ma_sound sound{};
	bool hasSound = false;
};
#endif