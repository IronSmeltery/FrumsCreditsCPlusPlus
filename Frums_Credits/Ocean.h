#ifndef OCEAN
#define OCEAN
#include "Misc.h"
#include "GlobalRNGDistributions.h"
#include "Console.h"
#ifndef oceanHeight
#define oceanHeight 10
#endif
inline constexpr char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
class Ocean {
	Distribution<int_dist> waveHeight_2t9, waveStay_1t15, letterTrash_25, trashChance_999, letterHeight_9, ephemeralY_N, ephemeralX_W;
	short currHeight{5}, targetHeight{5}, timeLeftAtHeight{1}, currWaveHeight{0};
	float trashPercent{0.0f},glitchPercent{0.0f};
public:
	using OceanData = std::array<std::string, oceanHeight>;
	bool waveProtection{true};
	ConsoleTypes::Color oceanColorAttr{ConsoleTypes::F_YELLOW};
private:
	std::array<std::array<char, nScreenWidth>, oceanHeight> oceanText{}, trashOverlay{};
	short GenerateWave();
	char GenerateTrash(float percent);
	void ShiftLeftAndAppend(std::array<char, nScreenWidth>& arr, char newValue);
public:
	Ocean() {};
	Ocean(SRef<Randomizer> randomizer_, float trashBasePercent, float glitchBasePercent);
	void AdvanceOcean();
	void ChangeTrashPercent(float percent);
	void ChangeGlitchPercent(float percent);
	//Ocean y-axis is up, opposite of console
	[[nodiscard]] OceanData GetOcean();
};
#endif