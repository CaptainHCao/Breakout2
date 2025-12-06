#include "Paddle.h"
#include <SDL3_image/SDL_image.h>

bool Paddle::load(SDL_Renderer* renderer) {
    texture = IMG_LoadTexture(renderer, "assets/paddle.png");
    if (!texture) return false;
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return true;
}

void Paddle::update(float dt, int logicalWidth, const bool* keys) {
    float move = 0.0f;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  move -= speed;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) move += speed;

    x += move * dt;

    if (x < -size) x = logicalWidth - size;
    else if (x > logicalWidth) x = 0.0f;
}

void Paddle::render(SDL_Renderer* renderer, int logicalWidth) {
    SDL_FRect src{ 0, 0, size, size };
    SDL_FRect dst{ x, y, size, size };

    SDL_RenderTexture(renderer, texture, &src, &dst);

    // wrap-around drawing
    if (x < size) {
        SDL_FRect wrap = dst;
        wrap.x = x + logicalWidth;
        SDL_RenderTexture(renderer, texture, &src, &wrap);
    }
    else if (x + size > logicalWidth - size) {
        SDL_FRect wrap = dst;
        wrap.x = x - logicalWidth;
        SDL_RenderTexture(renderer, texture, &src, &wrap);
    }
}

void Paddle::destroy() {
    if (texture) SDL_DestroyTexture(texture);
    texture = nullptr;
}
