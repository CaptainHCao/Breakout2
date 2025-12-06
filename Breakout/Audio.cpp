#include "Audio.h"
#include <SDL3/SDL.h>

MusicPlayer::MusicPlayer() {}
MusicPlayer::~MusicPlayer()
{
    shutdown();
}

bool MusicPlayer::init(const char* path)
{
    if (!SDL_LoadWAV(path, &spec, &buffer, &length))
    {
        SDL_Log("Failed to load music WAV '%s': %s", path, SDL_GetError());
        return false;
    }

    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (!device)
    {
        SDL_Log("Failed to open audio device: %s", SDL_GetError());
        return false;
    }

    stream = SDL_CreateAudioStream(&spec, &spec);
    if (!stream)
    {
        SDL_Log("Failed to create audio stream: %s", SDL_GetError());
        return false;
    }

    if (!SDL_BindAudioStream(device, stream))
    {
        SDL_Log("Failed to bind audio stream: %s", SDL_GetError());
        return false;
    }

    SDL_PutAudioStreamData(stream, buffer, (int)length);
    SDL_ResumeAudioDevice(device);

    initialized = true;
    enabled = true;
    return true;
}

void MusicPlayer::update()
{
    if (!initialized || !stream || !enabled)
        return;

    // If no more audio queued, loop the track
    if (SDL_GetAudioStreamAvailable(stream) == 0)
    {
        SDL_PutAudioStreamData(stream, buffer, (int)length);
    }
}

void MusicPlayer::setEnabled(bool on)
{
    enabled = on;

    if (!initialized || !device)
        return;

    if (enabled)
    {
        SDL_ResumeAudioDevice(device);
    }
    else
    {
        SDL_PauseAudioDevice(device);
    }
}

void MusicPlayer::shutdown()
{
    if (stream)
    {
        SDL_UnbindAudioStream(stream);
        SDL_DestroyAudioStream(stream);
        stream = nullptr;
    }

    if (device)
    {
        SDL_CloseAudioDevice(device);
        device = 0;
    }

    if (buffer)
    {
        SDL_free(buffer);
        buffer = nullptr;
    }

    initialized = false;
}