#ifndef FRUMS
#define FRUMS
#include "Misc.h"
#include "FrameTimer.h"

//Feel free to replace 'YourCustomTerminal' with your terminal or any output device of your choice
#include "Console.h"
#ifdef USE_WINDOWS_TERMINAL
#include "WindowsTerminal.h"
using Terminal = WindowsTerminal;
#elif defined YOURCUSTOMTERMINAL
#include "YourCustomTerminal.h"
using Terminal = YourCustomTerminal;
#endif


//Feel free to replace 'MiniaudioSoundEngine' with any audio system device of your choice
#include "SoundEngine.h"
#ifdef MINIAUDIO
using Sound = MiniaudioSoundEngine;
#elif defined CUSTOMSOUNDENGINE
using Sound = CustomSoundEngine;
#endif


#if not defined NO_TERMINAL_DEFINED and not defined NO_SOUND_ENGINE_DEFINED

#include "Scenes.h"
#include "Distributions.h"
class SceneManager {
public:
	static int Begin();
private:	
	long lastBeat{0}, beat{0};
	double lastSubdivTime{0.0}, subdivDuration{}; // absolute song time of last subdivision
	//const wchar_t ub{L'⠀'};	
	short currSceneNum{1};
	bool clearOnNextSceneRun{ false }, stopDuping{ false }, loopScene{ false }, formatSeconds{ true }, isRunning{ true }, paused{ false }, singleFrameUnpause{ false };
	const float bpm{179},secondsPerBeat{60.0f/bpm};
	int sceneTimeSubDiv{1};
	SRef<RandomizerWithGlobals> randomizer;
	SRef<Console> console;
	SRef<TimeData> time;
	URef<SoundEngine> audio;
	URef<SceneBase> scene;

#ifndef SHOWCASE_MODE
	FrameTimer timer;
#endif
#ifndef DISABLE_INPUT
	unsigned int skipIndex = 0;
#else 
	unsigned int skipIndex = 0;
#endif
	
	int Initialize();
	std::string FindCreditsName();
	int Run();
	// SwitchScene will clear the console and set the current scene to the new scene number. 
	// If songSyncTime is provided, it will also seek the audio to that time. 
	// -1 to sync to the new scene's StartTime, -2 for no syncing (keep current song time).
	// Invalid scene numbers are ignored.
	void SwitchScene(short newSceneNum, double songSyncTime = -2);
#ifdef SDL_WINDOW_CREATED
	void HandleEvents(bool& paused);
	//sets skipIndex to be at the start of the current scene when switching or restarting scenes
	void AdjustSkipIndex();
#endif
	void UpdateWindowTitle(int beatIndex, double songTime, int subDivIndex, double fps, double average_fps, std::wstring extraTitleBarInfo = { L"",false });
	void UpdateWindowTitle();
};
#endif // NO_TERMINAL_DEFINED or NO_SOUND_ENGINE_DEFINED
#endif // FRUMS