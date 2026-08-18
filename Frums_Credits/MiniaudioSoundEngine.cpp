#include "MiniaudioSoundEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
bool SoundEngine::musicIsPlaying = true;

MiniaudioSoundEngine::MiniaudioSoundEngine(int& result) {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cerr << "Failed to Initialize miniaudio engine!\n";
        result = false;
        return;
    }
    hasSound = false;
    result = true;
}

MiniaudioSoundEngine::~MiniaudioSoundEngine() {
    StopSound();
    ma_engine_uninit(&engine);
}

void MiniaudioSoundEngine::EngineLoop()
{
    if (hasSound && ma_sound_at_end(&sound)) musicIsPlaying = false;
}

bool MiniaudioSoundEngine::PlaySound(const std::string filePath) {
    StopSound(); // Stop current sound if any
    
    if (ma_sound_init_from_file(&engine, filePath.c_str(), 
#ifdef preDecodeAudio
        MA_SOUND_FLAG_DECODE | 
#endif
        MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, nullptr, &sound) != MA_SUCCESS) {
        std::cerr << "Failed to load sound: " << filePath << "\n";
        return false;
    }

    if (ma_sound_start(&sound) != MA_SUCCESS) {
        std::cerr << "Failed to start sound\n";
        ma_sound_uninit(&sound);
        return false;
    }

    musicIsPlaying = true;
    hasSound = true;
    return true;
}

void MiniaudioSoundEngine::StopSound() {
    if (hasSound) {
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
        hasSound = false;
        musicIsPlaying = false;
    }
}

void MiniaudioSoundEngine::Pause() {
    if (hasSound && IsPlaying()) {
        ma_sound_stop(&sound); // miniaudio uses stop as Pause
        musicIsPlaying = true;
    }
}

void MiniaudioSoundEngine::Resume() {
    if (hasSound && IsPaused()) {
        ma_sound_start(&sound);
        musicIsPlaying = true;
    }
}

bool MiniaudioSoundEngine::IsPlaying() const {
    return hasSound && ma_sound_is_playing(&sound);
}

bool MiniaudioSoundEngine::IsPaused() const {
    return hasSound && !ma_sound_is_playing(&sound);
}

double MiniaudioSoundEngine::GetSongTime() {
    if (!hasSound) return 0.0;

    ma_uint64 cursorFrame;
    if (ma_sound_get_cursor_in_pcm_frames(&sound, &cursorFrame) != MA_SUCCESS) return 0.0;

    ma_uint64 totalFrames;
    if (ma_sound_get_length_in_pcm_frames(&sound, &totalFrames) != MA_SUCCESS) return 0.0;

    float lengthSecondsFloat;
    if (ma_sound_get_length_in_seconds(&sound, &lengthSecondsFloat) != MA_SUCCESS) return 0.0;

    double lengthSeconds = static_cast<double>(lengthSecondsFloat);

    return lengthSeconds * (static_cast<double>(cursorFrame) / static_cast<double>(totalFrames));
}

bool MiniaudioSoundEngine::SeekSeconds(double seconds) {
    if (!hasSound) return false;

    ma_uint64 totalFrames;
    ma_sound_get_length_in_pcm_frames(&sound, &totalFrames); // Get total frames

    ma_uint64 targetFrame = static_cast<ma_uint64>(seconds * engine.sampleRate);
    if (targetFrame > totalFrames) targetFrame = totalFrames;

    return ma_sound_seek_to_pcm_frame(&sound, targetFrame) == MA_SUCCESS;
}

bool MiniaudioSoundEngine::SeekBeat(int beatIndex, double secondsPerBeat) {
    if (secondsPerBeat <= 0.0) return false;
    return SeekSeconds(beatIndex * secondsPerBeat);
}

bool MiniaudioSoundEngine::SetVolume(float volume) {
    return ma_engine_set_volume(&engine, volume) == MA_SUCCESS;
}

float MiniaudioSoundEngine::GetVolume()
{
    return ma_engine_get_volume(&engine);
}
