#include "Brick.h"
#include <SDL3_image/SDL_image.h>

bool Brick::loadTexture(SDL_Renderer* renderer, const char* path)
{
    texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        SDL_Log("Brick: failed to load texture '%s': %s", path, SDL_GetError());
        return false;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return true;
}

void Brick::hit()
{
    if (!alive)
        return;

    state++;

    if (state > maxState) {
        alive = false;
    }
}


void Brick::render(SDL_Renderer* renderer) const
{
    if (!alive)
        return;

    if (texture) {
        float texW = 0.0f, texH = 0.0f;
        SDL_GetTextureSize(texture, &texW, &texH);

        constexpr int SpriteStates = 3; // total frames in sprite sheet

        float frameWidth = texW / SpriteStates;

        // Clamp visual state so weak bricks still show last valid frame
        int visualState = SDL_clamp(state, 1, maxState);
        visualState = SDL_clamp(visualState, 1, SpriteStates);

        SDL_FRect src{
            frameWidth * (visualState - 1),
            0.0f,
            frameWidth,
            texH
        };

        SDL_RenderTexture(renderer, texture, &src, &rect);

        return;
    }

    // Debug fallback
    SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
    SDL_RenderRect(renderer, &rect);
}

void Brick::destroy()
{
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}
