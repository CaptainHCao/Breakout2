// UpgradePickup.h
#pragma once
#include <SDL3/SDL.h>
#include "UpgradeTypes.h"

struct UpgradePickup
{
    UpgradeType type;
    SDL_FRect   rect;
    float       vy = 120.0f;   // falling speed (logical pixels/sec)
    bool        alive = true;

    void update(float dt) { rect.y += vy * dt; }

    void render(SDL_Renderer* r) const
    {
        SDL_SetRenderDrawColor(r, 255, 255, 0, 255); // bright yellow
        SDL_RenderFillRect(r, &rect);
    }
};
