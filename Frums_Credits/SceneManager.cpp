#pragma warning(push)
#pragma warning(disable : 6031 4244 26819) 

#include "SceneManager.h"
#include "Misc.h"

#ifndef DISABLE_INPUT


#endif
#if not defined NO_TERMINAL_DEFINED and not defined NO_SOUND_ENGINE_DEFINED
int SceneManager::Begin() {
	return SceneManager().Initialize();
}


int SceneManager::Initialize() {
	int result{0};
	console = std::make_shared<Terminal>(result);
	if (result) return result;
	console->ChangeConsoleTitle(L"Frums - Credits");
	std::string creditsFileName = FindCreditsName();
	if (creditsFileName.empty()) {
		std::cout << "Could not find credits mp3 file in media folder.\n";
		// return EXIT_FAILURE;
	}
	audio = std::make_unique<Sound>(result);
	if (!result) return result;
	if (!audio->PlaySound((std::string("media/") + creditsFileName).c_str())) return EXIT_FAILURE;
	audio->Pause();
	if (!audio->SetVolume(globalVolume)) { std::cerr << "Sound engine was unable to lower volume.\n"; }
#ifdef SDL_WINDOW_CREATED

#ifdef useWideCharacters
	console->Write(0, 0, L"Press any key to start credits...", ConsoleTypes::F_GREEN);
#else
	console->Write(0, 0, "Press any key to start credits...", ConsoleTypes::F_GREEN);
#endif
#endif
	if (console->DisplayFrame()) return EXIT_FAILURE;
#ifdef SDL_WINDOW_CREATED
	SDL_Event sdlEvent;
	bool waitingForInput = true;
	while (waitingForInput) {
		if (!SDL_PollEvent(&sdlEvent)) continue;
		if (sdlEvent.type == SDL_QUIT) return EXIT_SUCCESS;
		if (sdlEvent.type == SDL_KEYDOWN) waitingForInput = false;
	}
#endif
	console->Clear();
	time = std::make_shared<TimeData>(0.0, secondsPerBeat);
	randomizer = std::make_shared<RandomizerWithGlobals>(Randomizer::MakeSeed(std::string("Frums - Credits")));
	audio->Resume();
#ifdef SHOWCASE_MODE
	SwitchScene(1);	
#else
	SwitchScene(skipTime[skipIndex].first, skipTime[skipIndex].second);
	//audio->SeekSeconds(skipTime[skipIndex].second); // starting offset for debugging 
#endif
	if (!scene.get()) {std::cerr << "Expected initial scene, got nullptr. Exiting.\n";return EXIT_FAILURE;}
#ifndef SHOWCASE_MODE
	timer.Start();
#endif
	return Run();
}

// Finds the song file as long as it has 'credits' in the name and is an mp3
std::string SceneManager::FindCreditsName() {
	std::string name;
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	std::filesystem::path exeDir = std::filesystem::path(path).remove_filename();
	std::filesystem::path mediaDir = exeDir / "../../media";
	for (const auto& entry : std::filesystem::directory_iterator(mediaDir)) {
		if (!entry.is_regular_file()) continue;
		std::wstring wname = entry.path().filename().wstring();

		// Lowercase for case-insensitive match
		std::wstring lower = wname;
		std::transform(lower.begin(), lower.end(), lower.begin(), towlower);

		if (entry.path().extension() == L".mp3" && lower.find(L"credits") != std::wstring::npos) {
			name = std::filesystem::path(wname).string();
			return name;
		}
	}
	return name;
}
void SceneManager::UpdateWindowTitle() {
#ifndef SHOWCASE_MODE
	UpdateWindowTitle(beat, time->songTime, sceneTimeSubDiv, timer.GetFPS(), timer.GetAverageFPS(), scene->extraTitleBarInfo());
#endif
}
void SceneManager::UpdateWindowTitle(int beatIndex, double songTime, int subDivIndex, double fps, double average_fps, std::wstring extraTitleBarInfo) {
#ifndef SHOWCASE_MODE
	//if (audio->IsPlaying()) {
		std::wstring title = L"";
		if (!scene->overrideTitleBar) {
			title += L"Beat/Passed/Time " + std::to_wstring(beatIndex) + L'/' + std::to_wstring(time->BeatsElapsed()) + L'/' + (formatSeconds ? FormatTime(songTime, 2) : ToXdp(songTime, 3) + L"s");
			//title += L" | Subdivision: " + std::to_wstring(subDivIndex);
			//title += L" | Time " + ToXdp(songTime, 3) + L"s";
#if not defined DISABLE_INPUT and not defined titleChangeAffectsConsoleWindowInsteadOfSdlWindow
			title += L" | SkipIndex " + std::to_wstring(skipIndex);
#endif
			title += L" | Prints " + std::to_wstring(std::dynamic_pointer_cast<WindowsTerminal>(console)->prints);
			title += L" | Fps/Avg " + std::to_wstring((int)fps) + L"/" + std::to_wstring((int)average_fps);
#ifndef titleChangeAffectsConsoleWindowInsteadOfSdlWindow
			title += L" | Scene " + std::to_wstring(currSceneNum);
#endif
			title += L"  ";
		}
		if (!extraTitleBarInfo.empty()) title += L" | " + extraTitleBarInfo;
		console->ChangeConsoleTitle(title);
	//}
#endif
}

int SceneManager::Run() {
	// Main Loop
	while (Sound::musicIsPlaying && isRunning) {
		audio->EngineLoop();
		/*Sdl,window,paused,scene switching*/{
#ifndef SHOWCASE_MODE
		if (!paused) timer.UpdateFrameTicks();
#endif
#ifdef SDL_WINDOW_CREATED
		HandleEvents(paused); // Handle SDL events
#endif
#ifndef SHOWCASE_MODE
		UpdateWindowTitle();
#endif
		if (paused || !scene.get()/* || !audio->IsPlaying()*/) continue;
		// Handle scene switches 
		if (scene->finished)
			SwitchScene(currSceneNum + !loopScene, loopScene? -1:-2);
		}
		if (!isRunning) break;
		/*Time calculation*/{
		time->songTime = audio->GetSongTime();

		// Only advance if enough time has passed
		if (time->songTime - lastSubdivTime < subdivDuration) continue;

		beat = time->SubdivIndex(sceneTimeSubDiv);
					
		// Prevent multiple calls per beat
		if (lastBeat == beat) continue;	

		// Advance to next subdivision
		lastSubdivTime += subdivDuration; 
		time->beat = lastBeat = beat;
		if (time->sceneStartBeat == 0) time->sceneStartBeat = time->beat;
		}
		/*Run current scene*/{
		if (clearOnNextSceneRun) {
			clearOnNextSceneRun = false;
			console->Clear();
		}
     	if (scene->Run()) console->DisplayFrame();
#ifndef SHOWCASE_MODE
		if (singleFrameUnpause) {
			singleFrameUnpause = false;
			paused = true;
			audio->Pause();
		}
#endif
		}
	}
	/*Exit*/{
	scene.reset();
	audio.reset();
	console.reset();
	SDL_Quit();
	return EXIT_SUCCESS;
	}
}



void SceneManager::SwitchScene(short newSceneNum, double songSyncTime) {
	if (newSceneNum <= 0 || newSceneNum > 9) return;
	if (songSyncTime != -2 && songSyncTime != -1 && songSyncTime < 0) return;
	audio->Pause();
	clearOnNextSceneRun = true;
	scene.reset();
	if (songSyncTime != -1) time->songTime = songSyncTime;
#define MakeScene(Scene) std::make_unique<Scene>(SceneConstructorDataWithoutTypes)
	switch (newSceneNum) {
	case 1: scene = MakeScene(Scene_1_Static); break;
	case 2: scene = MakeScene(Scene_2_Weather_1); break;
	case 3: scene = MakeScene(Scene_3_Blinky_Credits_Text_Rise); break;
	case 4: scene = MakeScene(Scene_4_Loading_Bar); break;
	case 5: scene = MakeScene(Scene_5_Diagnosis); break;
	case 6: scene = MakeScene(Scene_6_Weather_2); break;
	case 7: scene = MakeScene(Scene_7_Access_Points); break;
	case 8: scene = MakeScene(Scene_8); break;
	case 9: isRunning = false; return;
	default: return;
	}
#undef MakeScene
	//audio->Resume();
	currSceneNum = newSceneNum;
	// Sync subdivision time to current song time for new scene
	if (scene.get()) {
		bool speedrunMode = false;
		sceneTimeSubDiv = scene->TimeSubDiv();
		subdivDuration = secondsPerBeat / sceneTimeSubDiv;
		if (songSyncTime != -2) {
			audio->SeekSeconds(scene->StartTime());
			if (songSyncTime >= 0) {
				speedrunMode = true;
			}
		}
		//if (songSyncTime == -1) {
		//	audio->SeekSeconds(scene->StartTime());
		//}
		//else if (songSyncTime >= 0) {
		//	audio->SeekSeconds(songSyncTime);
		//	speedrunMode = true;
		//}
		time->songTime = scene->StartTime();
		lastSubdivTime = std::max(scene->StartTime() - subdivDuration, 0.0);
		beat = time->beat = time->SubdivIndex(sceneTimeSubDiv);
		lastBeat = std::min<unsigned int>(time->beat - 1, 0);
		//UpdateWindowTitle(time->beat, time->songTime, sceneTimeSubDiv, timer.GetFPS(), timer.GetAverageFPS(), scene->extraTitleBarInfo());
		time->sceneStartBeat = time->beat;
		
		scene->Init();
		time->sceneStartBeat = 0;
		//-------
		if (speedrunMode) {
			//double time_jump = time->songTime = lastSubdivTime = audio->GetSongTime();

		//-------
			unsigned int goal_beat = time->ToBeats(songSyncTime, sceneTimeSubDiv);
			unsigned int curr_beat = time->beat;
			//int simulated_beat = std::max(time->ToBeats(time->songTime, sceneTimeSubDiv), time->ToBeats(scene->StartTime(), sceneTimeSubDiv));
			
			//int skip_to_beat = time->ToBeats(lastSubdivTime, sceneTimeSubDiv);
			while (time->songTime < songSyncTime) {
				lastSubdivTime = time->songTime;
				time->songTime += subdivDuration;//time->ToSongTime(curr_beat, sceneTimeSubDiv));
				time->beat = time->SubdivIndex(sceneTimeSubDiv);
				if (time->sceneStartBeat == 0) time->sceneStartBeat = time->beat;
				if (clearOnNextSceneRun) {
					clearOnNextSceneRun = false;
					console->Clear();
				}
				scene->Run();
				curr_beat++;
			}
			//time->songTime = lastSubdivTime;
			console->DisplayFrame();
			//-------
			audio->SeekSeconds(songSyncTime);
			time->songTime = audio->GetSongTime();
			lastSubdivTime = std::max(time->songTime - subdivDuration, 0.0);
			beat = time->beat = time->SubdivIndex(sceneTimeSubDiv)-1;
			lastBeat = std::max<unsigned int>(time->beat - 2, 0);

		//lastBeat = time->SubdivIndex(sceneTimeSubDiv) - 1;
		}
	}
	if (!paused) audio->Resume();
	console->prints = 0;
}

#ifdef SDL_WINDOW_CREATED
void SceneManager::HandleEvents(bool& paused) {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			Sound::musicIsPlaying = false;
			audio->StopSound();
			break;
		case SDL_KEYDOWN:
			if (!stopDuping) continue;
			stopDuping = false;
			switch (sdlEvent.key.keysym.scancode) {
				/*Pause/Resume*/
			case SDL_SCANCODE_SPACE: {
				paused ? audio->Resume() : audio->Pause();
				paused = !paused;
				break;
			}
				/*Restart scene*/
			case SDL_SCANCODE_R: {
				AdjustSkipIndex();
				SwitchScene(currSceneNum, -1);
				if (paused) {
					paused = false;
					audio->Resume();
				}
				break;
			}

				/*Next scene*/
			case SDL_SCANCODE_RIGHT: {
				if (currSceneNum < 8) { 
					SwitchScene(currSceneNum + 1, -1); }
					AdjustSkipIndex();
				if (paused) {
					paused = false;
					audio->Resume();
				}
				break;
			}
				/*Prev scene*/
			case SDL_SCANCODE_LEFT: {
				if (currSceneNum > 1) { 
					SwitchScene(currSceneNum - 1, -1); }
					AdjustSkipIndex();
				if (paused) {
					paused = false;
					audio->Resume();
				}
				break;
			}
				/*Next skip index*/
			case SDL_SCANCODE_UP: {
				if (skipIndex < std::size(skipTime) - 1) {
					skipIndex++;
					//audio->SeekSeconds(skipTime[skipIndex].second);
					//currSceneNum = skipTime[skipIndex].first;
					SwitchScene(skipTime[skipIndex].first, skipTime[skipIndex].second);
					if (paused) {
						paused = false;
						audio->Resume();
					}
				}
				break;
			}
				/*Prev skip index*/
			case SDL_SCANCODE_DOWN: {
				if (skipIndex > 0) {
					skipIndex--;
					//audio->SeekSeconds(skipTime[skipIndex].second);
					//currSceneNum = skipTime[skipIndex].first;
					SwitchScene(skipTime[skipIndex].first, skipTime[skipIndex].second);
					if (paused) {
						paused = false;
						audio->Resume();
					}
				}
				break;
			}
			case SDL_SCANCODE_KP_GREATER:
				/*Single frame unpause*/

			default: break;
			}
		case SDL_KEYUP:
			if (stopDuping) continue;
			stopDuping = true;
			switch (sdlEvent.key.keysym.scancode) {
				case SDL_SCANCODE_GRAVE:	scene->debugBits[SceneBase::db_tilde] = true; break;
				case SDL_SCANCODE_1:		scene->debugBits[SceneBase::db_1] = true; break;
				case SDL_SCANCODE_2:		scene->debugBits[SceneBase::db_2] = true; break;
				case SDL_SCANCODE_3: 		scene->debugBits[SceneBase::db_3] = true; break;
				case SDL_SCANCODE_4: 		scene->debugBits[SceneBase::db_4] = true; break;
				case SDL_SCANCODE_5: 		scene->debugBits[SceneBase::db_5] = true; break;
				case SDL_SCANCODE_6: 		scene->debugBits[SceneBase::db_6] = true; break;
				case SDL_SCANCODE_7: 		scene->debugBits[SceneBase::db_7] = true; break;
				case SDL_SCANCODE_8: 		scene->debugBits[SceneBase::db_8] = true; break;
				case SDL_SCANCODE_9: 		scene->debugBits[SceneBase::db_9] = true; break;
				case SDL_SCANCODE_0: 		scene->debugBits[SceneBase::db_0] = true; break;
				case SDL_SCANCODE_MINUS:	scene->debugBits[SceneBase::db_min] = true; break;
				case SDL_SCANCODE_EQUALS:	scene->debugBits[SceneBase::db_equal] = true; break;
				case SDL_SCANCODE_SLASH:	loopScene = !loopScene; break;		
				case SDL_SCANCODE_SEMICOLON: formatSeconds = !formatSeconds; UpdateWindowTitle(); break;
					/*Return to current skip index*/
				case SDL_SCANCODE_S: {
					//audio->SeekSeconds(skipTime[skipIndex].second);
					SwitchScene(currSceneNum, skipTime[skipIndex].second);
					break;
				}
				case SDL_SCANCODE_PERIOD: {
				if (paused) {
					paused = false;
					audio->Resume();
					singleFrameUnpause = true;
				}
				break;
				}
			case SDL_SCANCODE_COMMA: {
				//currently doesn't work. skips back multiple frames sometimes
				if (paused && time->songTime > 0) {
					//time->songTime = audio->GetSongTime();
					unsigned int curr_beat = time->ToBeats(time->songTime, scene->TimeSubDiv());

					//double t = time->ToSongTime(time->ToBeats(time->songTime, scene->TimeSubDiv()) - 1, scene->TimeSubDiv());
					//double t_diff = time->songTime - t;
					//double s = time->songTime - 0.3351955307 / scene->TimeSubDiv();
					//double s_diff = time->songTime - s;
					SwitchScene(currSceneNum, audio->GetSongTime());
					//audio->Pause();
				}
				break;
				}
			}
		}
	}
}
void SceneManager::AdjustSkipIndex() {
	switch (currSceneNum) {
	case 1: skipIndex =  0; break;
	case 2: skipIndex =  1; break;
	case 3: skipIndex =  5; break;
	case 4: skipIndex = 10; break;
	case 5: skipIndex = 12; break;
	case 6: skipIndex = 14; break;
	case 7: skipIndex = 17; break;
	case 8: skipIndex = 20; break;
	}
}
#endif
#endif // NO_TERMINAL_DEFINED or NO_SOUND_ENGINE_DEFINED
#pragma warning(pop)