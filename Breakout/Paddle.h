#pragma once
#include <SDL3/SDL.h>

class Paddle {
public:
    float x = 0.0f;
    float y = 280.0f;
    float speed = 150.0f;
    float size = 32.0f;
    SDL_Texture* texture = nullptr;

    bool load(SDL_Renderer* renderer);
    void update(float dt, int logicalWidth,const bool* keys);
    void render(SDL_Renderer* renderer, int logicalWidth);
    void destroy();
};
