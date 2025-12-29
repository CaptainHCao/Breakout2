#pragma once
#include <SDL3/SDL.h>
#include "UpgradeTypes.h"

struct Brick {
    SDL_FRect rect{ 0, 0, 0, 0 };
    SDL_Texture* texture = nullptr;

    bool alive = true;

    int state = 1;        // current damage state (1-based)
    int maxState = 3;     // durability (1..3)

    bool hasUpgrade = false;
    UpgradeType upgradeType{};

    bool loadTexture(SDL_Renderer* renderer, const char* path);
    void render(SDL_Renderer* renderer) const;
    void hit();
    void destroy();
};

