#pragma once
#include <SDL3/SDL_audio.h>

class MusicPlayer
{
public:
    MusicPlayer();
    ~MusicPlayer();

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