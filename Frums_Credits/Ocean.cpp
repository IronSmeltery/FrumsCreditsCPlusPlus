#include "Ocean.h"



Ocean::Ocean(SRef<Randomizer> randomizer_, float trashBasePercent, float glitchBasePercent)/*: randomizer(randomizer_)*/ {
	ChangeTrashPercent(trashBasePercent);
	ChangeGlitchPercent(glitchBasePercent);
	waveHeight_2t9 = Distribution<int_dist>(*randomizer_, 2, 9);
	waveStay_1t15 = Distribution<int_dist>(*randomizer_, 1, 15);
	letterTrash_25 = Distribution<int_dist>(*randomizer_, 0, static_cast<int>(sizeof(alphabet)) - 2);
	letterHeight_9 = Distribution<int_dist>(*randomizer_, 0, 9);
	trashChance_999 = Distribution<int_dist>(*randomizer_, 0, 999);
	ephemeralY_N = Distribution<int_dist>(*randomizer_, 0, 9); // random row
	ephemeralX_W = Distribution<int_dist>(*randomizer_, 0, nScreenWidth - 1);  // random column
	for (short i = 0; i < nScreenWidth; i++) AdvanceOcean();
}

short Ocean::GenerateWave() {
	//this function makes a random walk
	//for more python-accurate waves, see get_ocean_slices() in the original's ocean.py line 68 

	if (currHeight == targetHeight) {
		if (timeLeftAtHeight > 0) {
			timeLeftAtHeight--;
			return currHeight;
		}
		else {
			timeLeftAtHeight = std::min(waveStay_1t15.Rand(), waveStay_1t15.Rand());
			do {
				targetHeight = waveHeight_2t9.Rand();
			} while (targetHeight == currHeight);
		}
	}
	return currHeight > targetHeight ? --currHeight : ++currHeight;
}

char Ocean::GenerateTrash(float percent) {
	//rn we're just pluggin in arbitrary fractions into this function
	//for more python-accurate waves, see mutate_text() in the original's ocean.py line 47
	return (trashChance_999.Chance(percent)) ? at_literal(alphabet, letterTrash_25.Rand()) : 0;
}

void Ocean::AdvanceOcean() {
	currWaveHeight = GenerateWave(); // store current wave
	short waveHeight = currWaveHeight;

	// Advance ocean base sourceText
	for (short y = 0; y < oceanText.size(); y++) {
		ShiftLeftAndAppend(oceanText[y], y > waveHeight ? ' ' : y < waveHeight ? '.' : '#');
	}

	// Advance persistent trash overlay
	bool alreadyTrashed = false;
	for (short y = 0; y < trashOverlay.size(); y++) {
		if (!(waveProtection && y == waveHeight) && !alreadyTrashed) {
			char trash = GenerateTrash(trashPercent);
			if (trash != 0) {
				ShiftLeftAndAppend(trashOverlay[y], trash);
				alreadyTrashed = true; // only one trash per frame if desired
				continue;
			}
		}
		// Otherwise append empty space
		ShiftLeftAndAppend(trashOverlay[y], ' ');
	}
}



void Ocean::ChangeTrashPercent(float percent) { trashPercent = Clamp(percent); }
void Ocean::ChangeGlitchPercent(float percent) { glitchPercent = Clamp(percent); }

[[nodiscard]] Ocean::OceanData Ocean::GetOcean() {
	OceanData returnData;
	//const float ephemeralPercent = /*0.2f /*/ glitchPercent; // 20% chance per cell
	bool anyEphemeralThisFrame = false;

	for (size_t y = 0; y < oceanText.size(); ++y) {
		// Copy base ocean row
		returnData[y].assign(oceanText[y].begin(), oceanText[y].end());

		for (size_t x = 0; x < nScreenWidth; ++x) {
			char cell = returnData[y][x];
			char overlay = trashOverlay[y][x]; // persistent trash

			// Ephemeral static if no persistent trash and waveProtection allows
			if (overlay == ' ' && !(cell == '#' && waveProtection)) {
				char ret = GenerateTrash(glitchPercent);
				if (ret) {
					overlay = ret;
					anyEphemeralThisFrame = true;
				}
			}

			// Apply overlay if any
			if (overlay != ' ') {
				cell = overlay;
			}

			returnData[y][x] = cell;
		}
	}

	// Guarantee at least one ephemeral character per frame
	if (!anyEphemeralThisFrame) {
		size_t y = ephemeralY_N.Rand();
		size_t x = ephemeralX_W.Rand();
		if (!(returnData[y][x] == '#' && waveProtection)) {
			returnData[y][x] = at_literal(alphabet,letterTrash_25.Rand());
		}
	}
	return returnData;
}



void Ocean::ShiftLeftAndAppend(std::array<char, nScreenWidth>& arr, char newValue) {
	std::move(arr.begin() + 1, arr.end(), arr.begin());
	arr.back() = newValue;
}
