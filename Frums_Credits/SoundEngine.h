#ifndef SOUNDENGINE
#define SOUNDENGINE
#include "Misc.h"
#include "Defines.h"

struct SoundEngine {
	static bool musicIsPlaying;
	virtual void EngineLoop() {}; //this gets called every iteration of the scenemanager::run() loop
	virtual bool PlaySound(std::string filePath) = 0;
	virtual void StopSound() = 0;
	virtual double GetSongTime() = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;
	virtual bool IsPaused() const = 0;
	virtual bool IsPlaying() const = 0;
	virtual bool SeekSeconds(double seconds) = 0;
	virtual bool SeekBeat(int beatIndex, double secondsPerBeat) = 0;
	virtual bool SetVolume(float volume) = 0;
	virtual float GetVolume() = 0;
	virtual ~SoundEngine() = default;
};

#ifdef MINIAUDIO
#include "MiniaudioSoundEngine.h"
#elif defined CUSTOMSOUNDENGINE
#include "CustomSoundEngine.h"
#else
#error "No sound engine defined. Please define either MINIAUDIO or CUSTOMSOUNDENGINE."
#define NO_SOUND_ENGINE_DEFINED
#endif
#endif
