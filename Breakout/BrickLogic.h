#pragma once
#include <SDL3/SDL.h>
#include <vector>

struct Brick {
    SDL_FRect rect;
    bool alive = true;
};

// Function declaration
std::vector<Brick> spawnBricks(int screenWidth, int screenHeight, int brickW, int brickH, int margin = 10, int padding = 4);
