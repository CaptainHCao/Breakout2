// UpgradePickup.h
#pragma once
#include <SDL3/SDL.h>
#include "UpgradeTypes.h"

struct UpgradePickup
{
    UpgradeType type;
    SDL_FRect   rect;
    float       vy = 100.0f;
    bool        alive = true;

    SDL_Texture* texture = nullptr;

    // --- animation ---
    int   frameCount = 5;
    int   currentFrame = 0;
    float frameTime = 0.2f;    
    float frameTimer = 0.0f;

    int frameW = 0;
    int frameH = 0;

    void initAnimation()
    {
        if (texture)
        {
            float texW, texH;
            SDL_GetTextureSize(texture, &texW, &texH);
            frameW = texW / frameCount;
            frameH = texH;
        }
    }

    void update(float dt)
    {
        rect.y += vy * dt;

        // animate
        frameTimer += dt;
        if (frameTimer >= frameTime)
        {
            frameTimer -= frameTime;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }

    void render(SDL_Renderer* r) const
    {
        if (!texture)
        {
            SDL_RenderFillRect(r, &rect);
            return;
        }

        SDL_FRect src{
            (float)(currentFrame * frameW),
            0.0f,
            (float)frameW,
            (float)frameH
        };

        SDL_RenderTexture(r, texture, &src, &rect);
    }
};
