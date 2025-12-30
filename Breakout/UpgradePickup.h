// UpgradePickup.h
#pragma once
#include <SDL3/SDL.h>
#include "UpgradeTypes.h"

struct UpgradePickup
{
    UpgradeType type;
    SDL_FRect   rect;
    float       vy = 100.0f;   // falling speed (logical pixels/sec)
    bool        alive = true;

    SDL_Texture* texture = nullptr;

    void update(float dt) { rect.y += vy * dt; }

    void render(SDL_Renderer * r) const {
        if (texture) {
            SDL_RenderTexture(r, texture, nullptr, &rect);
        }
        else {
            SDL_RenderFillRect(r, &rect);
        }
    }
};
