// SDLApp.h
#pragma once
#include <SDL3/SDL.h>
#include "Audio.h"   // for MusicPlayer

struct SDLApp
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    int width = 1600;
    int height = 900;
    int logicalWidth = 640;
    int logicalHeight = 320;

    MusicPlayer music;

    // Initialise SDL, window, renderer, logical presentation, audio
    bool init(const char* title = "Breakout Game");

    // Destroy everything and quit SDL
    void shutdown();
};
