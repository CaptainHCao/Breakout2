#pragma once
#include <SDL3/SDL.h>

class Paddle {
public:
    bool load(SDL_Renderer* renderer);
    void update(float dt, int logicalWidth);
    void render(SDL_Renderer* renderer, int logicalWidth) const;
    void destroy();

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    SDL_FRect getRect() const;

private:
    float x = 0.0f;
    float y = 280.0f;

    float width = 64.0f;   // defaults, will be overwritten by texture size
    float height = 16.0f;

    float speed = 160.0f;
    SDL_Texture* texture = nullptr;
};
