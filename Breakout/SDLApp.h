#pragma once
#include <SDL3/SDL.h>

struct SDLApp {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int width = 1600;
    int height = 900;
    int logicalWidth = 640;
    int logicalHeight = 320;

    bool init();
    void shutdown();
};
