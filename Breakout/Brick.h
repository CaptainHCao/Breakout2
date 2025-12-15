// Brick.h
#pragma once
#include <SDL3/SDL.h>
#include "UpgradeTypes.h"


struct Brick {
    SDL_FRect rect{ 0, 0, 0, 0 };
    bool alive = true;
    SDL_Texture* texture = nullptr;

	bool hasUpgrade;
    UpgradeType upgradeType;

    bool loadTexture(SDL_Renderer* renderer, const char* path);
    void render(SDL_Renderer* renderer) const;
    void destroy();
};
