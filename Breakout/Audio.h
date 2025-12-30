#pragma once
#include <SDL3/SDL_audio.h>

class SoundEffect
{
public:
    SoundEffect() = default;
    ~SoundEffect() { shutdown(); }

    SoundEffect(const SoundEffect&) = delete;
    SoundEffect& operator=(const SoundEffect&) = delete;
    SoundEffect(SoundEffect&&) = delete;
    SoundEffect& operator=(SoundEffect&&) = delete;

    bool load(const char* path);
    void play();
    void shutdown();
    void stop();

private:
    SDL_AudioSpec spec{};
    Uint8* buffer = nullptr;
    Uint32 length = 0;

    SDL_AudioDeviceID device = 0;
    SDL_AudioStream* stream = nullptr;
};

class MusicPlayer
{
public:
    MusicPlayer();
    ~MusicPlayer();

    MusicPlayer(const MusicPlayer&) = delete;
    MusicPlayer& operator=(const MusicPlayer&) = delete;
    MusicPlayer(MusicPlayer&&) = delete;
    MusicPlayer& operator=(MusicPlayer&&) = delete;

    // Load a WAV file and prepare for playback
    bool init(const char* path);

    // Call once per frame to keep looping
    void update();

    // Turn music on/off
    void setEnabled(bool enabled);

    // Free all resources
    void shutdown();

private:
    SDL_AudioSpec spec{};
    Uint8* buffer = nullptr;
    Uint32 length = 0;

    SDL_AudioDeviceID device = 0;
    SDL_AudioStream* stream = nullptr;

    bool enabled = true;
    bool initialized = false;
};