#include "Paddle.h"
#include <SDL3_image/SDL_image.h>

bool Paddle::load(SDL_Renderer* renderer) {
    texture = IMG_LoadTexture(renderer, "assets/paddle.png");
    if (!texture) {
        SDL_Log("Failed to load paddle texture: %s", SDL_GetError());
        return false;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    // Get the actual texture size 
    float texW = 0.0f, texH = 0.0f;
    if (SDL_GetTextureSize(texture, &texW, &texH)) {
        width = texW;
        height = texH;
    }
    else {
        SDL_Log("SDL_GetTextureSize failed: %s", SDL_GetError());
    }

    return true;
}

void Paddle::update(float dt, int logicalWidth) {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float move = 0.0f;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  move -= speed;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) move += speed;

    x += move * dt;

    // wrap-around using width
    if (x < -width)              x = logicalWidth - width;
    else if (x > logicalWidth)   x = 0.0f;
}

void Paddle::render(SDL_Renderer* renderer, int logicalWidth) const {
    if (!texture) return;

    SDL_FRect src{ 0.0f, 0.0f, width, height };
    SDL_FRect dst{ x, y,        width, height };

    SDL_RenderTexture(renderer, texture, &src, &dst);

    // wrap-around drawing on both sides
    if (x < width) {
        SDL_FRect wrap = dst;
        wrap.x = x + logicalWidth;
        SDL_RenderTexture(renderer, texture, &src, &wrap);
    }
    else if (x + width > logicalWidth - width) {
        SDL_FRect wrap = dst;
        wrap.x = x - logicalWidth;
        SDL_RenderTexture(renderer, texture, &src, &wrap);
    }
}

SDL_FRect Paddle::getRect() const {
    SDL_FRect r;
    r.x = x;
    r.y = y;
    r.w = width;
    r.h = height;
    return r;
}

void Paddle::destroy() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Paddle::makeFaster(float factor)
{
    speed *= factor;

    // optional safety clamp
    if (speed > 600.0f)
        speed = 600.0f;
}

void Paddle::makeBigger(float factor)
{
    float centerX = x + width * 0.5f;

    width *= factor;

    // keep paddle centered
    x = centerX - width * 0.5f;

    // optional clamp so it never gets ridiculous
    if (width > 200.0f)
        width = 200.0f;
}
