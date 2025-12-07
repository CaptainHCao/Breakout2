//defines what a Brick is
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

void Brick::render(SDL_Renderer* renderer) const
{
    if (!alive) return;

    if (texture) {
        SDL_RenderTexture(renderer, texture, nullptr, &rect);
        return;
    }

    // solid brick
    SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255);
    SDL_RenderFillRect(renderer, &rect);

    // outline
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

