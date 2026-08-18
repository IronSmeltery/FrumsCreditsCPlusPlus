#include "Scenes.h"
Scene_1_Static::Scene_1_Static(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes) {}
void Scene_1_Static::Init() {
	symbol_3 = Distribution<int_dist>(*randomizer, 0, 2);
	screenH_24 = Distribution<int_dist>(*randomizer, 0, nScreenHeight - 1);
	screenW_40 = Distribution<int_dist>(*randomizer, 0, nScreenWidth/2);
}
bool Scene_1_Static::Run() {
	if (time->songTime < 2.85f) return false;
	if (time->songTime >= S1_SceneEnd) {
 		finished = true;
		return false;
	}
	
	int beat_0 = 68; //what point in the song beat 0 is in the original code
	int b = time->beat - beat_0;
	WriteNoise((int)pow(b, 1.4), b, std::max(70 - b, 0), std::max(4 * (40 - b), 0));
	return true;
}
void Scene_1_Static::WriteNoise(int amount, int white, int light_grey, int dark_grey) {
	randomizer->int_gen.ChangeDist(0, white + light_grey + dark_grey - 1);
	for (int i = 0; i < amount; i++) {
		char n = symbol_3.Rand();
		int col = randomizer->int_gen.Rand();
		ConsoleTypes::Color color = (col < white) ? ConsoleTypes::F_WHITE_BRIGHT : (col < white + light_grey) ? ConsoleTypes::F_WHITE : ConsoleTypes::F_GRAY;
		console->Write(screenW_40.Rand() * 2, screenH_24.Rand(), (n == 1 ? L"##" : n == 2 ? L"@@" : L"  "), color);
	}
}

Scene_2_Weather_1::Scene_2_Weather_1(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes), typeWriter(randomizer) {}
void Scene_2_Weather_1::Init() {
	ocean = Ocean(randomizer, 0.004f, 1.0f / (1 << 9));
	ocean.oceanColorAttr = ConsoleTypes::F_CYAN;
	typeWriter.SetGlitchPercentage(0); 
}
bool Scene_2_Weather_1::Run() {
	if (time->songTime < S2_SceneStart) return false;

	bool typewriterUpdated{ false }, clearText{ false }, oceanUpdated{ false }, oceanColorChange{ false }, textChange{ false }, oceanTrashIncrease{ false };

	switch (time->beat) {
	case 377: clearText = textChange = textBlockID = S2_ocean_2; doType = false; break;
 	case 379: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_BLUE; break;
 	case 381: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_CYAN; break;
	case 383: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_BLUE; break;
	case 665: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_GRAY; break;
	case 387: doType = true; break;
	case 720: clearText = textChange = textBlockID = S2_ocean_3; doType = false; break;
	case 725: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_YELLOW, ocean.ChangeGlitchPercent(1.0f/(1<<8)); break;
	case 730: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_YELLOW_BRIGHT; break;
	case 930: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_YELLOW; break;
	case 726: doType = true; break;
	case 1070: clearText = textChange = textBlockID = S2_ocean_4; doType = false; break;
	case 1071: doType = true; break;
	case 1110: ocean.waveProtection = false; break;
	case 1075: oceanColorChange = ocean.oceanColorAttr = ConsoleTypes::F_GRAY; break;
	default: break;
	}
	if (time->beat >= 1137 && typeWriter.finished) finished = true;
	else if (time->beat >= 1110) ocean.ChangeGlitchPercent(MapRange((float)time->beat, 1110, 1137, 1.0f / (1 << 8), 1));
	else if (time->beat <= 135 && textBlockID != S2_ocean_1) doType = clearText = textChange = true, textBlockID = S2_ocean_1;
	if (textChange) {
		std::vector<std::wstring> strings;
		if (!ReadFileSegment(L"Strings.txt", textBlockID, L"//", strings)) throw "why";
		typeWriter.SetText(strings);
	}


	if (time->beat % 2 == 1) {
		ocean.AdvanceOcean();
		Ocean::OceanData data = ocean.GetOcean();

		for (short i = 0; i < data.size(); i++) {
			console->Write(0, nScreenHeight - i - 1, data[i], ocean.oceanColorAttr);
		}
		oceanUpdated = true;
	}
	

	if (clearText) console->ClearRect(2, 1, nScreenWidth - 3, std::max(textTypedIndexY, (short)1), true);
	if (doType) {
		typewriterUpdated = true;
		std::vector<strType> sourceText = typeWriter.GetText();
		if (sourceText.empty()) return false;
		textTypedIndexY = static_cast<short>(sourceText.size());
		for (short i = 0; i < static_cast<short>(sourceText.size()); i++) {
			console->Write(2, 1 + i, sourceText[i], ConsoleTypes::F_WHITE_BRIGHT);
		}
		lastTypeTime = lastSongTime;
		typeWriter.Type();
	}
	return oceanUpdated || oceanColorChange || typewriterUpdated || clearText;
}

Scene_3_Blinky_Credits_Text_Rise::Scene_3_Blinky_Credits_Text_Rise(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes),
    weather(randomizer, 21, 9, 2009), cb(console, 4, 8, false), staggeredWriter(randomizer) {
}
void Scene_3_Blinky_Credits_Text_Rise::Init() {	
	weather.Update();
	glitch_999 = Distribution<int_dist>(*randomizer, 0, 999);
	std::vector<std::wstring> strings;
	if (!ReadFileSegment(L"Strings.txt", S3_rising_weather, L"//", strings)) throw "why";
	staggeredWriter.SetText(strings);	

	weatherTextPositions.reserve(47);
	std::function<void(int, int, int)> setData = [this](int x, int y, int length) { 
		for (int i = 0; i < length; i++) {
			weatherTextPositions.push_back(std::pair{ x + i, y });
		}
	};
	setData(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 1, 14);//weather type
	setData(weatherBoxTopLeftX +  2, weatherBoxTopLeftY + 3,  4);//temp
	setData(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 3,  4);//"Wind"
	setData(weatherBoxTopLeftX + 18, weatherBoxTopLeftY + 3,  2);//speed
	setData(weatherBoxTopLeftX + 21, weatherBoxTopLeftY + 3,  3);//"mph"
	setData(weatherBoxTopLeftX + 25, weatherBoxTopLeftY + 3,  2);//dir
	setData(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 5, 13);//"precipitation"
	setData(weatherBoxTopLeftX + 16, weatherBoxTopLeftY + 6,  6);//precip
	sceneEndFadeTracker.reserve(nScreenSize / 2);
	for (int i = 0; i < sceneEndFadeTracker.capacity(); i++) {
		sceneEndFadeTracker.push_back(i * 2);
	}
}
bool Scene_3_Blinky_Credits_Text_Rise::Run() {//143.2 per second
	unsigned int index = time->SubdivIndex(TimeSubDiv());
	if (time->beat >= 3060) {
		finished = true; 
		return false;
	}
	int start_subdiv = time->ToBeats(48.1666, 8); //should be 1149
	bool ret{ false };
	if (time->songTime >= 123.15) fadeOutActive = true;

	if (blinkIntroActive) ret |= BlinkyIntro(time->beat, start_subdiv);
	else if (scrollTextActive) ret |= ScrollText();
	if (fadeOutActive) ret |= FadeOut();
	
	return ret;
}
bool Scene_3_Blinky_Credits_Text_Rise::BlinkyIntro(unsigned int index, unsigned int start_subdiv, int boxOverride) {
	bool sceneReturn{ false };
	if (boxOverride == 0) {
		if (index >= start_subdiv) {
			int offset = ((cbi < 12) ? 4 : 2);
			if (index >= (unsigned int)last_inputline_subdiv + offset or first_box) {
				cb.GetNextState();
				last_inputline_subdiv += (first_box) ? 0 : offset;
				cbi++;
				cbi %= 20;
				first_box = false;
				if (cbi == 0) { centerBoxLoops++; }
				sceneReturn = true;
			}
		}
	}
	else centerBoxLoops = boxOverride;

	//interior of box is from (x:y) 61:14 to 88:19 inclusive
	switch (centerBoxLoops) {
	case 1:console->Write(2, 1, "animation  | plaaosert", ConsoleTypes::F_CYAN_BRIGHT);
		   console->Write(2, 2, "c++ remake | IronSmeltery & LARAUJO", ConsoleTypes::F_CYAN_BRIGHT); centerBoxLoops++; sceneReturn = true; break;
	case 3:console->Write(2, 3, "bgm        | Frums - Credits", ConsoleTypes::F_CYAN_BRIGHT); centerBoxLoops++; sceneReturn = true; break;
	case 5:console->Write(2, 5, "running C++ 17", ConsoleTypes::F_CYAN); centerBoxLoops++; sceneReturn = true;  break;
	case 7:console->Write(2, 6, "in the windows terminal", ConsoleTypes::F_CYAN); centerBoxLoops++; sceneReturn = true; break;
	case 9:console->Write(0, nScreenHeight - 3, std::string(nScreenWidth, L'-'), ConsoleTypes::F_WHITE); centerBoxLoops++; sceneReturn = true; break;
	case 11:console->Write(0, nScreenHeight - 2, "  > _", ConsoleTypes::F_WHITE); centerBoxLoops++; sceneReturn = true; break;
	case 13:
		console->Write(weatherBoxTopLeftX, weatherBoxTopLeftY, std::string(29, L'-'), ConsoleTypes::F_WHITE);
		for (short y = weatherBoxTopLeftY + 1; y < weatherBoxTopLeftY + 7; y++) {
			console->Write(weatherBoxTopLeftX, y, "|", ConsoleTypes::F_WHITE);
			console->Write(nScreenWidth - 1, y, "|", ConsoleTypes::F_WHITE);
		}
		centerBoxLoops++; sceneReturn = true; break;
	case 15:console->Write(weatherBoxTopLeftX + 2, weatherBoxTopLeftY + 1, weather.GetDate(), ConsoleTypes::F_YELLOW_BRIGHT); centerBoxLoops++; sceneReturn = true; break;/*date*/
	case 17:console->Write(weatherBoxTopLeftX, weatherBoxTopLeftY + 2, std::string(29, L'-'), ConsoleTypes::F_WHITE); centerBoxLoops++; sceneReturn = true; break;
	case 19:/*temp, wind speed & dir*/{
		auto temp = weather.GetTemp();
		console->Write(weatherBoxTopLeftX + 2, weatherBoxTopLeftY + 3, temp.first, temp.second);
		console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 3, weather.GetWindLine(), ConsoleTypes::F_CYAN_BRIGHT);
	}
	centerBoxLoops++; sceneReturn = true; break;
	case 21:/*precipitation*/{
		console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 5, "Precipitation", ConsoleTypes::F_BLUE_BRIGHT);
		std::wstring precipt = weather.GetPrecipt();
		console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 6, std::wstring(9 - precipt.length(), L' ') + precipt, ConsoleTypes::F_BLUE_BRIGHT);//% doesnt move, xx.x or xx.xx shifts left and keeps % in the same spot
	}
	centerBoxLoops++; sceneReturn = true; break;
	case 23:/*weather*/
		blinkIntroActive = false;
		scrollTextActive = true;
		console->Write(weatherBoxTopLeftX + 12, weatherBoxTopLeftY + 1, weather.GetWeatherType(), ConsoleTypes::F_YELLOW);
		console->ClearRect(2, 1, (nScreenWidth / 2) + 4, weatherBoxTopLeftY);
		centerBoxLoops++; sceneReturn = true; break;
	default: break;
	}
	return sceneReturn;
}
bool Scene_3_Blinky_Credits_Text_Rise::ScrollText() {
	bool updateInputLine{ false }, updateTextHistory{ false }, updateWeatherBox{ false }, clearText{ false };
	for (int db = debugControls::db_tilde; db < debugControls::ENUM_END; db++) {
		if (!debugBits[db]) continue;
		switch (db) {
		case debugControls::db_tilde: updateInputLine = true; debugBits[db_tilde] = false; break;
		case debugControls::db_1: break;
		case debugControls::db_2: break;
		case debugControls::db_3: break;
		case debugControls::db_4: break;
		case debugControls::db_5: break;
		case debugControls::db_6: break;
		case debugControls::db_7: break;
		case debugControls::db_8: break;
		case debugControls::db_9: break;
		case debugControls::db_0: break;
		case debugControls::db_min: break;
		case debugControls::db_equal:  break;
		default: break;
		}
	}

	/*
	= text 2
	= electric boogaloo
	- more text
	- yet more text
	------------
	> 
	
	*/

	if (time->songTime < 80.3333333333333333333) { return false; }

	if (first_text_scroll) {
		last_weather_subdiv = last_inputline_subdiv = time->SubdivIndex(TimeSubDiv());
		updateInputLine = true;
		first_text_scroll = false;
	}

	//first funding section starts at 77.4301676 seconds
	//doing subdiv++ because this section runs on 8th-beats and so does the scene
	int subdiv = time->SubdivIndex(TimeSubDiv());
	lastSongTime = time->songTime;
	if (subdiv == last_inputline_subdiv) { return false; }
	if (subdiv >= last_inputline_subdiv + 1) {
		last_inputline_subdiv++;
		updateInputLine = true;
	}

	if (subdiv == last_weather_subdiv) { return false; }
	if (subdiv >= last_weather_subdiv + weatherInterval) {
		last_weather_subdiv += weatherInterval;
		updateWeatherBox = true;
	}

	if (updateWeatherBox) {
		switch (weatherStage) {
		case 1: //update weatherbox once per 8 beats
		case 2: //update weatherbox once per 4 beats
			weather.IncrementDay();
			weather.Update();
			PrintWeather(1);
			break;
		case 3: //cheese line, make date jump ahead to 2010
			/*
			idea 1:
			- have cheese happen immedeately (check in input line code sets interval to 1)
			- update weatherbox goes to case 3 on the next frame
			- cheese() gets called
			- set last weather subdiv to whatever the current subdiv is
			- also in case 3, set weather interval to 7 or 8
			- next update (7 or 8 frames later) set up randomizer part, set interval to 4, and set stage to 4

			idea 2:
			- first part same as idea 1
			- when setting up the randomizer part for stage 4, set the interval to 6
			- on first stage 4 weather update, change the interval to 4 and continue as normal

			idea 3:
			- detect cheese in input line code, but keep the weather interval at 32
			- wait for subdiv to be >= to last weather subdiv + 32
			- then call cheese(), set up stage 4, and set the interval to 8
			- on first stage 4 weather update, change the interval to 4 and continue as normal
			*/



			if (cheese == 1) {
				Cheese(); 
				cheese++;
				weatherInterval = 8;
				last_weather_subdiv = subdiv;
			}
			else if (cheese >= 2) {
				weather.year = 2010;
				weatherInterval = 4;
				weatherStage = 4;
			}
			break;	
		case 4: //update weatherbox once per half-beat, randomize dates
			if (cheese == 2) {
				console->Write(weatherBoxTopLeftX + 12, weatherBoxTopLeftY + 1, L"          ", ConsoleTypes::F_WHITE);
				console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 6, L"          ", ConsoleTypes::F_WHITE);
				cheese++;
			}
			weather.RandomizeDayMonth();
			weather.Update();
			PrintWeather(2);
			break;
		case 5: //make date jump ahead to 2011-04-03
			weather.day = 2;
			weather.month = 3;
			weather.year = 2011;
			weatherStage++;
			weatherInterval = 1;
			weather.Update();
			PrintWeather(3);
			break;
		case 6: //update weatherbox every 8th-beat
			weather.IncrementDay();
			weather.Update();
			PrintWeather(4);
			weather_increments++; //debug thingy
			if (weather.month == 6 and weather.day == 15) {
				weatherStage++;
				}
			break;
		default: 
			break;
		}
		
	}


	//if the input line at the bottom needs to update
	if (updateInputLine) {
		staggeredWriter.Type();
		if (staggeredWriter.GetText() == inputLine.first) { updateInputLine = false; }
		else {
			inputLine.first = staggeredWriter.GetText();
			if (staggeredWriter.queue_finished) scrollTextActive = false;
			if (inputLine.first == L"=      Corpo") { 
				cheese = true;
				//DBOUT("Youfuckedup2");
				weatherInterval = 1;
				weatherStage = 3; }
			if (inputLine.first == L"-................................") { 
				weatherStage = 5; }
			if (inputLine.first == L"!!!!!") {
				//DBOUT(L"time at log clear:" + std::to_wstring(time->songTime));
				console->ClearRect(2, 1, weatherBoxTopLeftX - 2, historyLog.size);
				historyLog.Clear();
				inputLine = { L"  ", ConsoleTypes::F_WHITE };
				weatherStage = 2;
				weatherInterval = 32;
				return true;
			}
			console->Write(4, nScreenHeight - 2, inputLine.first.substr(1) + strType(nScreenWidth - 3, L' '), ConsoleTypes::F_WHITE);
		}
	}
	
	//if the scrolling history long needs to update
	//occurs when staggeredWriter has finished the current line and depleted its wait_timer
	//updateTextHistory = updateInputLine and staggeredWriter.end_of_line == 1;
	updateTextHistory = staggeredWriter.end_of_line == 1 and staggeredWriter.wait_timer == 0 and not staggeredWriter.queue_finished;
	if (updateTextHistory) {
		staggeredWriter.end_of_line++;
		//insert space and remove leading spaces
		size_t firstNonSpace = inputLine.first.find_first_not_of(L" ", 1);
		if (firstNonSpace == -1) { inputLine.first = L"= "; }
		else { inputLine.first = strType{ inputLine.first[0] } + L" " + inputLine.first.substr(firstNonSpace); }
		//change colour based on first character
		switch (inputLine.first[0]) {
		case L'=': inputLine.second = ConsoleTypes::F_GRAY; inputLine.first[0] = L' '; break;
		case L'-': inputLine.second = ConsoleTypes::F_YELLOW; break;
		case L'>': inputLine.second = ConsoleTypes::F_GREEN; break;
		default: inputLine.second = ConsoleTypes::Color(ConsoleTypes::F_GRAY | ConsoleTypes::B_RED_BRIGHT); break;
		}
		//truncate excess characters if the final string is too long
		int max_length = weatherBoxTopLeftX - 3;
		if (inputLine.first.length() > max_length) {
			inputLine.first = inputLine.first.substr(0, max_length); }
		//adds spaces to overwrite previous text if the final string is too short
		if (inputLine.first.length() < max_length) {
			inputLine.first += strType(max_length - inputLine.first.length(), L' ');
		}
		//transfer completed input line to history log and scroll the text
		historyLog.PushPop(inputLine);
		for (int i = 0; i < historyLog.size; i++) {
			auto& textPair = historyLog.Peek((historyLog.GetIndex() + historyLog.size - i) % historyLog.size);
			console->Write(2, (nScreenHeight - 5 - i), textPair.first.substr(0, (size_t)weatherBoxTopLeftX - 3), textPair.second);
		}
	}
	return updateInputLine || updateTextHistory || clearText || updateWeatherBox;

}
bool Scene_3_Blinky_Credits_Text_Rise::FadeOut() {
	//rapid fire 123.15
	//whole scene starts dying 127.65
	//finished 128.4
	switch (glitchStage) {
	case 1: 
		if ((time->songTime) >= 125.583) glitchStage++;
		break;
	case 2: {
		PrintWeather(5);
		float percentGlitch = Percent(static_cast<float>(time->songTime), 125.583f, 127.3f);
		if (percentGlitch < 1.0f) {
			for (int i = 0; i < 47; i++) {
				if (glitch_999.Chance(percentGlitch)) {
					console->Write(weatherTextPositions[i].first, weatherTextPositions[i].second, L' ', ConsoleTypes::F_WHITE);
				}
			}
		}
		else {
			console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 1, L"              ", ConsoleTypes::F_WHITE);
			console->ClearRect(weatherBoxTopLeftX + 2, weatherBoxTopLeftY + 3, 25, 4);
			glitchStage++;

			sceneEndFadeTracker.reserve(nScreenSize / 2);
			for (int i = 0, test = 1000; i < sceneEndFadeTracker.capacity(); i++, test--) {
				sceneEndFadeTracker[i] = i * 2;
				if (test <= 0) __debugbreak();
			}
		}
		}
		break;
	case 3:
		if ((time->songTime) >= 127.65) glitchStage++;
		break;
	case 4: {
		for (int i = 0, val, limit = (int)MapRange(static_cast<float>(time->songTime), 127.65f, 128.4f, 1, (float)sceneEndFadeTracker.size()); i < limit && !finished; i++) {
			if (sceneEndFadeTracker.size() > 1) {
				int index = randomizer->int_gen.ChangeDistAndRand(0, (int)sceneEndFadeTracker.size()-1);
				val = sceneEndFadeTracker[index];
				sceneEndFadeTracker.erase(sceneEndFadeTracker.begin() + index);
			}
			else val = sceneEndFadeTracker[0];
 			console->Write(val % nScreenWidth, val / nScreenWidth, L"  ", ConsoleTypes::F_WHITE);
			if (sceneEndFadeTracker.size() <= 1) {
				finished = true;
				glitchStage++;
			}
		}
	} break;
	default: break;
	}
	return true;
}
void Scene_3_Blinky_Credits_Text_Rise::PrintWeather(int id) {
	if (glitchStage > 2) return;
	console->Write(weatherBoxTopLeftX + 2, weatherBoxTopLeftY + 1, weather.GetDate(), ConsoleTypes::F_YELLOW_BRIGHT);
	auto temp = weather.GetTemp();
	console->Write(weatherBoxTopLeftX + 2, weatherBoxTopLeftY + 3, temp.first, temp.second);
	console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 3, weather.GetWindLine(), ConsoleTypes::F_CYAN_BRIGHT);
	console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 5, L"Precipitation", ConsoleTypes::F_BLUE_BRIGHT);
	std::wstring precipt = weather.GetPrecipt();
	precipt = std::wstring(6 - precipt.length()/2, L' ') + precipt;
	precipt += std::wstring(13 - precipt.length(), L' ');
	console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 6, precipt, ConsoleTypes::F_BLUE_BRIGHT);
	console->Write(weatherBoxTopLeftX + 12, weatherBoxTopLeftY + 1, weather.GetWeatherType(), ConsoleTypes::F_YELLOW);
}
void Scene_3_Blinky_Credits_Text_Rise::Cheese() {
	console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 6, L"   cheese%   ", ConsoleTypes::F_BLUE_BRIGHT);
	console->Write(weatherBoxTopLeftX + 2, weatherBoxTopLeftY + 1, L"Connection lost...       ", ConsoleTypes::F_RED);
	console->Write(weatherBoxTopLeftX + 2, weatherBoxTopLeftY + 3, L"-1°F   ", ConsoleTypes::F_RED_BRIGHT);
	console->Write(weatherBoxTopLeftX + 13, weatherBoxTopLeftY + 3, L"WIND -1 mph NW", ConsoleTypes::F_CYAN_BRIGHT);
}

Scene_4_Loading_Bar::Scene_4_Loading_Bar(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes), sw(randomizer) {}
void Scene_4_Loading_Bar::Init() {	
	ocean = Ocean(randomizer, 1.0f / (1 << 8), 1.0f / (1 << 9));
	ocean.oceanColorAttr = ConsoleTypes::F_MAGENTA;
	ocean.waveProtection = false;
	hextet_15 = Distribution<int_dist>(*randomizer, 0, 15);
	std::vector<std::wstring> strings;
	if (!ReadFileSegment(L"Strings.txt", S4_loading_bar, L"//", strings)) throw "why";
	sw.SetText(strings);
}
bool Scene_4_Loading_Bar::Run() {
	if (time->songTime < 134.0) return false;
	bool progress_bar_update{ false }, staggered_writer_update{ false }, ocean_hextet_update{ false }, other_update{ false };

	if (time->beat >= slow_bar_start and time->beat < error_bar_start and time->beat >= last_progress_bar_subdiv + (unsigned int)8) {
		last_progress_bar_subdiv += 8;
		progress_bar_update = true; 
	}
	if (time->beat >= text_start) { staggered_writer_update = true;	}
	if (time->beat >= error_bar_start and time->beat <= ocean_start) { progress_bar_update = true; }
	if (time->beat >= ocean_start and time->beat < fast_bar_start) { ocean_hextet_update = true; }
	if (time->beat >= fast_bar_start) { progress_bar_update = true; }
	
	//single-time events
	switch (time->beat) {
	case 3199: //initial box
		DrawLoadingBarBox();
		progress_bar_update = true;
		break;
	case 3200: //initial box text
		console->Write(16, 10, L"Loading...", ConsoleTypes::F_GRAY);
		other_update = true;
		break;
	case 3443: //error print
		console->Write(2, 1, L"The system has encountered a fatal error. Please wait.", ConsoleTypes::F_RED_BRIGHT);
		console->Write(2, 3, L"[ERR: 801]", ConsoleTypes::F_RED_BRIGHT);
		other_update = true;
		break;
	case 3447: //clear screen and reprint box
		console->Clear();
		DrawLoadingBarBox();
		progress_bar = 0;
		progress_bar_update = true;
		break;
	case 3448: //second box text
		console->Write(14, 10, L"Please wait...", ConsoleTypes::F_GRAY);
		other_update = true;
		break;
	}
	if (time->beat >= 3455) {
		console->Clear();
		progress_bar_update = staggered_writer_update = ocean_hextet_update = other_update = false;
		finished = true;
	}

	//if the "annual financial support" line below the progress bar needs to update
	if (staggered_writer_update) {
		sw.Type();

		if (sw.GetText() == financial_line) { staggered_writer_update = false; }
		else {
			financial_line = sw.GetText();
			console->Write(6, 12, financial_line + strType(24 - financial_line.length(), L' '), ConsoleTypes::F_GRAY);
		}
	}

	if (progress_bar_update) {
		if (time->beat < error_bar_start) {	SlowLoadingBar(); }
		else if (time->beat >= error_bar_start and time->beat <= ocean_start) { RunawayLoadingBar(); }
		else { FastLoadingBar(); }
	}

	if (ocean_hextet_update) {
		HextetErr();
		ocean.AdvanceOcean();
		Ocean::OceanData data = ocean.GetOcean();
		for (short i = 0; i < data.size(); i++) {
			console->Write(0, nScreenHeight - i - 1, data[i], ocean.oceanColorAttr);
		}
	}
	/*
	slow loading bar (light yellow)
	annual financial support staggered writer (grey)
	"loading..." in loading bar (grey, progress #s go on top of it)

	red glitchy text takes over loading bar (increments by rand_int(40, 70))
	purple ocean (10% glitch chance)
	red hextet block
	"the system has enountered a fatal error. please wait" (red)
	"[err: 801]" (red)

	clear screen
	fast loading bar (green, increments by rand_int(4,6))
	"please wait..." in loading bar (grey, progress #s go on top of it)
	*/
	//console->(std::vector<int>{ 1, 2, 3 }));
	return progress_bar_update || staggered_writer_update || ocean_hextet_update || other_update || true;
}
void Scene_4_Loading_Bar::HextetErr() {
	for (short y = 0; y < 4; y++) {
		for (short x = 0; x < 8; x++) {
			console->Write(2 + x * 5, 5 + y,
				std::wstring{ at_literal(hexAlphabet, hextet_15.Rand()) } +
							  at_literal(hexAlphabet, hextet_15.Rand()) +
							  at_literal(hexAlphabet, hextet_15.Rand()) +
							  at_literal(hexAlphabet, hextet_15.Rand()), ConsoleTypes::F_RED_BRIGHT);
		}
	}
}
void Scene_4_Loading_Bar::SlowLoadingBar() {
	progress_bar = static_cast<unsigned char>(std::min(progress_bar + 1, 30));
	console->Write(6, 10, std::wstring(progress_bar, L'#'), ConsoleTypes::F_YELLOW_BRIGHT);
}
void Scene_4_Loading_Bar::RunawayLoadingBar() {
	progress_bar += randomizer->int_gen.ChangeDistAndRand(40, 70);
	std::wstring str(progress_bar, L'#');
	str = FuckUpString(str, 0.40f, randomizer);
	ConsoleExtensions::WriteWrap(*(console.get()), 6, 10, str, ConsoleTypes::F_RED_BRIGHT);
}
void Scene_4_Loading_Bar::FastLoadingBar() {
	progress_bar += randomizer->int_gen.ChangeDistAndRand(4, 6);
	console->Write(6, 10, std::wstring(progress_bar, L'#'), ConsoleTypes::F_GREEN_BRIGHT);
}
void Scene_4_Loading_Bar::DrawLoadingBarBox() {
	console->Write(4, 9, std::wstring(34, L'-'), ConsoleTypes::F_WHITE);
	console->Write(4, 10, L'|', ConsoleTypes::F_WHITE);
	console->Write(37, 10, L'|', ConsoleTypes::F_WHITE);
	console->Write(4, 11, std::wstring(34, L'-'), ConsoleTypes::F_WHITE);
	//basically just draws this:
	//L"----------------------------------";
	//L"|                                |";
	//L"----------------------------------";
	//L"|         Please wait...         |";
	//L"|           Loading...           |";
}

Scene_5_Diagnosis::Scene_5_Diagnosis(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes) {}
void Scene_5_Diagnosis::Init() {
	std::vector<std::wstring> strings;
	if (!ReadFileSegment(L"Strings.txt", S5_diagnosis, L"//", strings)) throw "why";
	sw1.SetText(strings);
	if (!ReadFileSegment(L"Strings.txt", S5_searching, L"//", strings)) throw "why";
	sw2.SetText(strings);
	if (!ReadFileSegment(L"Strings.txt", S5_date, L"//", strings)) throw "why";
	sw3.SetText(strings);
}
bool Scene_5_Diagnosis::Run() {
	if (time->songTime < 144.8) { return false; }
	bool diagnosis_line_update{ false }, search_update{ false }, date_update{ false }, history_log_update{ false }, pbs_mode{ false }, other_update{false};


	if (time->songTime > 163.3833333) pbs_mode = true;

	if (sw3.end_of_line == 1 and sw3.wait_timer <= 1) {
		sw3.end_of_line++;
		history_log_update = true; }
	if (time->beat > 3457) {
		diagnosis_line_update = true;
		search_update = true;
		date_update = true;
	}


	//single time events
	switch (time->beat) { 
	case 3457: //should be 144.8666667 s
		console->Write(2, 1, L"Automatic diagnosis unsuccessful. Please await.", ConsoleTypes::F_GRAY);
		console->Write(2, 2, L'>', ConsoleTypes::F_GRAY);
		console->Write(0, nScreenHeight - 10, std::wstring(nScreenWidth, L'-'), ConsoleTypes::F_WHITE);
		console->Write(2, nScreenHeight - 2, L'$', ConsoleTypes::F_WHITE);
		other_update = true;
		break;
	case 3899:
		break;
	default: break;
	}

	if (diagnosis_line_update) {
		sw1.Type();

		if (sw1.GetText() == diagnosis_line) { diagnosis_line_update = false; }
		else {
			diagnosis_line = sw1.GetText();
			//if (diagnosis_line[0] == L'|') __debugbreak();
			//clearstr.copy(diagnosis_line.data() + diagnosis_line.length(), 60 - diagnosis_line.length(), 0);
			console->Write(4, 2, diagnosis_line + std::wstring(60 - diagnosis_line.length(), L' '), ConsoleTypes::F_RED);
		}
	}

	if (search_update) {
		sw2.Type();

		if (sw2.GetText() == search_line) { search_update = false; }
		else {
			search_line = sw2.GetText();
			//if (search_line[0] == L'|') __debugbreak();
			console->Write(8, 5, search_line + std::wstring(38 - search_line.length(), L' '), ConsoleTypes::F_YELLOW_BRIGHT);
		}		
	}

	if (date_update) {//luckily this scene only happens in one month so it doesn't need to be too complicated

		sw3.Type();

		if (sw3.GetText() == date_line) { search_update = false; }
		else {
			last_date_subdiv = time->beat;
			date_line = sw3.GetText();
			if (date_line.length() < 18) history_log_update = false;
 			//if (date_line[0] == L'|') __debugbreak();
			console->Write(4, nScreenHeight - 2, date_line + strType(18 - date_line.length(), L' '), ConsoleTypes::F_CYAN_BRIGHT);
		}		
#if 0
		switch (date_len) {
		case 0: 
			day++;
			if (day < 10) date_line[1] = std::to_wstring(day)[0];
			else date_line.replace(0, 2, std::to_wstring(day));
			date_len += 3; 
			break;
		case 3: date_len += 3; break;
		case 6: date_len += 4; break;
		case 10: date_len += 8; break;
		case 18: date_len = 0; break;
		}
		console->Write(4, nScreenHeight - 2, date_line.substr(0, date_len) + std::wstring(18- date_len, L' '), ConsoleTypes::F_CYAN_BRIGHT);
#endif
	}

	if (history_log_update) {
		if (date_line[0] == L'|') { historyLog.PushPop({ L"> " + date_line, ConsoleTypes::F_GREEN }); }
		else if (date_line[0] == L' ') { historyLog.PushPop({ L"  " + date_line, ConsoleTypes::BLACK }); }
		else { historyLog.PushPop({ L"- " + date_line, ConsoleTypes::F_YELLOW }); }
		for (int i = 0; i < historyLog.size; i++) {
			auto& textPair = historyLog.Peek((historyLog.GetIndex() + historyLog.size - i) % historyLog.size);
			console->Write(2, (nScreenHeight - 3 - i), textPair.first, textPair.second);
		}

	}

	if (time->songTime >= S5_SceneEnd) { 
		console->Clear(); 
		finished = true; }
	return diagnosis_line_update || search_update || date_update || history_log_update || other_update;
}

Scene_6_Weather_2::Scene_6_Weather_2(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes) {}
void Scene_6_Weather_2::Init() {
	ocean = Ocean(randomizer, 1.0f / (1 << 5), 1.0f / (1 << 7));
	ocean.oceanColorAttr = ConsoleTypes::F_GRAY;
	typeWriter.SetGlitchPercentage(0);//red text//43

	std::vector<std::wstring> strings, strings2;
	if (!ReadFileSegment(L"Strings.txt", S6_ocean_5, L"//", strings)) throw "why";
	strings2 = strings;
	strings2[strings2.size()-1] += FuckUpString(L"rrrrrrrrr", 0.8f, randomizer);
	typeWriter.SetText(strings2);
	for (auto& s : strings) s.erase(std::remove(s.begin(), s.end(), L'~'), s.end());

	//10%, 20%, 25%
	//90%,100%,100%
	for (short arrIdx = 0; arrIdx < 3; arrIdx++) {//array
		glitchCells[arrIdx].resize(strings.size());
		float glitch_chance_1 = (arrIdx == 2 ? 0.25f : arrIdx == 1 ? 0.2f : 0.1f);
		float glitch_chance_2 = arrIdx ? 1.0f : 0.9f;
		for (short vecIdx = 0; vecIdx < strings.size(); vecIdx++) {//vector
			if (strings[vecIdx].empty()) continue;
			glitchCells[arrIdx][vecIdx] = FuckUpString(std::wstring(strings[vecIdx].size(), 1), glitch_chance_1, randomizer) + L" ";
			if (vecIdx == strings.size() - 1) glitchCells[arrIdx][vecIdx] += FuckUpString(L"rrrrrrrrr", glitch_chance_2, randomizer);
		}
	}	
}
bool Scene_6_Weather_2::Run() {
	if (time->songTime < 166.0666667) { return false; }
	if (time->songTime >= S6_SceneEnd) { finished = true; return false; }
	bool oceanUpdated{ false }, doType{ false };
	/* in python original:
	on 3896, the scene starts (the text starts printing and ocean starts moving)
	- ocean_c_0 renders if (int(b ** 1.143) % 20 != 0, 8, 17, or 15)
	- ocean_c_0 renders if (int(b ** 1.2) % 20 == 0 or 15)
	- ocean_c_0 renders if (int(b ** 1.2) % 20 == 8))
	- ocean_c_0 renders if (int(b ** 1.2) % 20 == 17)
	- b is the "internal_beat" of the scene, which equals the full song beat - 3896
	on 4400, ocean_c_0 (the unglitched text copy) gets set to clear
	on 4401, 4403, 4405, 4407, and 4409 ocean randomly changes colours between black and dark grey (effectively flickering)
	on 4411, the ocean permanently disappears
	on 4413, the scene ends
	*/

	//0 = no render, 1 = normal text, 2-4 = glitched text
	short text_render_mode = 0;
	const unsigned short relative_beat = time->BeatsElapsed(/*-1*/); //may need to change this offset
	const int exp_1143 = static_cast<int>(pow(static_cast<float>(relative_beat), 1.143f)) % 20;
	const int exp_12 = static_cast<int>(pow(static_cast<float>(relative_beat), 1.2f)) % 20;
	if (exp_1143 != 0 and exp_1143 != 8 and exp_1143 != 15 and exp_1143 != 17) { text_render_mode = 1; }
	if (exp_12 == 0 or exp_12 == 15) { text_render_mode = 2; }
	if (exp_12 == 8) { text_render_mode = 3; }
	if (exp_12 == 17) { text_render_mode = 4; }
	//if (time->BeatsElapsed(-1) != time->beat - 3962) { __debugbreak(); }

	if (relative_beat) { typeWriter.Type(); }
	if (text_render_mode) {
		if (text_render_mode == 1 and time->beat >= 4466) {
			console->ClearRect(2, 1, 44, 10);
		}
		else { PrintText(text_render_mode - 2); }
		doType = true;
	}
	
	if (time->beat % 2 == 0 and time->beat < 4477) {
		ocean.AdvanceOcean();
		Ocean::OceanData data = ocean.GetOcean();
		
		if (time->beat > 4467 and randomizer->int_gen.ChangeDistAndChance(0, 1, 0.5f)) { 
			console->ClearRect(0, nScreenHeight - 10, nScreenWidth, 10); }
		else {
			for (short i = 0; i < data.size(); i++) {
				console->Write(0, nScreenHeight - i - 1, data[i], ocean.oceanColorAttr);
			}
		}
		oceanUpdated = true;
	}
	if (time->beat >= 4479) { finished = true; }
	return oceanUpdated || doType;
}
void Scene_6_Weather_2::PrintText(short glitchIdx) {
	if (glitchIdx < -1 or glitchIdx > 2) throw "why";
	std::vector<std::wstring> textVec = typeWriter.GetText();
	if (glitchIdx > -1) {
		for (short vecIdx = 0; vecIdx < textVec.size(); vecIdx++) {
			for (short charIdx = 0; charIdx < textVec[vecIdx].length(); charIdx++) {
				if (glitchCells[glitchIdx][vecIdx][charIdx] != 1) {
					if (textVec[vecIdx][charIdx] == L'\n') __debugbreak();
					textVec[vecIdx][charIdx] = glitchCells[glitchIdx][vecIdx][charIdx];
				}
			}
		}
	}
	for (short i = 0; i < static_cast<short>(textVec.size()); i++) {
		console->Write(2, 1 + i, textVec[i], ConsoleTypes::F_RED_BRIGHT);
	}
}

Scene_7_Access_Points::Scene_7_Access_Points(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes) {}
void Scene_7_Access_Points::Init() {
	std::vector<std::wstring> strings;
	if (!ReadFileSegment(L"Strings.txt", S7_access_points, L"//", strings)) throw "why";
	sw.SetText(strings);
}
bool Scene_7_Access_Points::Run() {
	if (time->songTime < S7_SceneStart) { return false; }
	if (time->songTime >= S7_SceneEnd) { finished = true; }
	bool fade_in{false}, sending_line_update{ false }, access_point_update{ false }, other_update{ false };
	if ((time->beat - 4527) % 2 == 0 and time->beat <= 4547) { fade_in = true;}
	if (time->songTime >= 193.05 ) { sending_line_update = true; }
	if (time->songTime >= 193.05 and time->beat % 8 == 7 && ping < 112) { access_point_update = true; }

	if (time->beat == 4527) {
		console->Write(2, nScreenHeight - 7, L"No access points are broadcasting.", ConsoleTypes::F_GRAY);
		console->Write(2, nScreenHeight - 6, L"Manual search in progress.", ConsoleTypes::F_GRAY);
		console->Write(2, nScreenHeight - 5, L"Last search 23.06.2021 (1882 days ago)", ConsoleTypes::F_GRAY);
		console->Write(0, nScreenHeight - 4, std::wstring(nScreenWidth, L'-'), ConsoleTypes::F_WHITE_BRIGHT);
		console->Write(2, nScreenHeight - 3, L"Sending >", ConsoleTypes::F_WHITE_BRIGHT);
	}

	if (sending_line_update) {
		sw.Type();

		if (sw.GetText() == sending_line) { sending_line_update = false; }
		else {
			sending_line = sw.GetText();
			console->Write(12, nScreenHeight - 3, sending_line + std::wstring(46 - sending_line.length(), L' '), ConsoleTypes::F_YELLOW);
		}
	}

	if (fade_in) {
		console->ClearRect(2,1,57,15,true);//7*6+3*5
		for (unsigned char count = 0; count < 72; count++) {
			bool show = randomizer->int_gen.ChangeDistAndRand(0, std::max(1, (int)(32 - pow(time->beat - 4527, 1.2f)))) < 4;
			bool last = time->beat >= 4547;

			if (show or last) {


				int x = 2 + ((count / 3) % 6) * 10;
				int y = 1 + (count % 3) + (count / 18) * 4;
				std::wstring str;
				switch (count % 3) {
				case 0: str = L"  ###  "; break;
				case 1: { short num = (count - 1) / 3 + 1;
						str = std::wstring(L"PBS #") + ((num < 10)? L"0" : L"") + std::to_wstring(num); } break;
				case 2: str = L"Unknown"; break;
				}
				console->Write(x, y, str, (last)? ConsoleTypes::F_RED : ConsoleTypes::F_GRAY);
			}
		}
	}

	if (access_point_update) {
		//ping counter
		ping++;
		bool pong = ping % 8 == 1;
		
		if (pong) {
			int x2 = x1;
			int y2 = y1;
			x1 = 2 + (((ping - 1) / 8) % 6) * 10;
			y1 = 1 + (((ping - 1) / 48) % 6) * 4;
			
			short num = (ping - 1) / 8 + 1;
			std::wstring pbs_string = std::wstring(L"PBS #") + ((num < 10) ? L"0" : L"") + std::to_wstring(num);
			console->Write(x1, y1,   L"  ###  ", ConsoleTypes::F_YELLOW);
			console->Write(x1, y1+1, pbs_string, ConsoleTypes::F_YELLOW);
			console->Write(x1, y1+2, L"Ping  1", ConsoleTypes::F_YELLOW);
			if (num > 1) {
				pbs_string = std::wstring(L"PBS #") + ((num-1 < 10) ? L"0" : L"") + std::to_wstring(num-1);
				console->Write(x2, y2,     L"  ...  ", ConsoleTypes::F_GRAY);
				console->Write(x2, y2 + 1, pbs_string, ConsoleTypes::F_GRAY);
				console->Write(x2, y2 + 2, L"-------", ConsoleTypes::F_GRAY);
			}
		}
		else if (ping == 112) {
			x1 = 2 + (((ping - 1) / 8) % 6) * 10;
			y1 = 1 + (((ping - 1) / 48) % 6) * 4;
			
			console->Write(12, 9,     L"  @@@  ", ConsoleTypes::F_GREEN);
			console->Write(12, 9 + 1, L"PBS #14", ConsoleTypes::F_GREEN);
			console->Write(12, 9 + 2, L" Active", ConsoleTypes::F_GREEN);			
		}
		else {
			int x = 8 + (((ping - 1) / 8) % 6) * 10;
			int y = 3 + (((ping - 1) / 48) % 6) * 4;
			std::wstring n = std::to_wstring((ping - 1) % 8 + 1);			
			console->Write(x, y, n, ConsoleTypes::F_YELLOW);
		}

	}
	return fade_in || sending_line_update || access_point_update || other_update;
}

Scene_8::Scene_8(SceneConstructorDataWithTypes): SceneBase(SceneConstructorDataWithoutTypes), cb(console, 12, 3, true) {}
void Scene_8::Init() {
	std::vector<std::wstring> strings;
	if (!ReadFileSegment(L"Strings.txt", S8_outro, L"//", strings)) throw "why";
	sw.SetText(strings);
}
bool Scene_8::Run() {
	if (time->songTime < S8_SceneStart) { return false; }
	if (time->songTime >= S8_SceneEnd) { finished = true; }
	if (time->beat > 6575) { return false; }
	
	bool centerbox_update{ false }, staggered_writer_update{ time->beat >= 5857 };

	int offset = ((cbt < 12) ? 4 : 2);
	if (time->beat >= (unsigned int)last_centerbox_subdiv + offset or first_box) {
		cb.GetNextState();
		last_centerbox_subdiv += (first_box) ? 0 : offset;
		cbt++;
		cbt %= 20;
		first_box = false;
		centerbox_update = true;
	}
	
	if (staggered_writer_update) {
		sw.Type();

		if (sw.GetText() == sending_line) { staggered_writer_update = false; }
		else if (sw.linesProcessed < 3) {
			sending_line = sw.GetText();
			console->Write(2, 1 + sw.linesProcessed, sending_line, ConsoleTypes::F_YELLOW);
		}
		else {
			sending_line = sw.GetText();
			std::wstring str2 = sending_line;
			std::transform(sending_line.begin(), sending_line.end(), str2.begin(), 
				[](wchar_t c) { return (isalnum(c)) ? L'-' : c; });
			console->Write(2, 1 + sw.linesProcessed - 3, str2 + std::wstring(46 - sending_line.length(), L' '), ConsoleTypes::F_YELLOW);
			console->Write(2, nScreenHeight - 4 + sw.linesProcessed - 3, sending_line, ConsoleTypes::F_YELLOW);
		}
	}

	if (time->beat == 6575) { console->Clear(); }

	return centerbox_update || staggered_writer_update;
}
