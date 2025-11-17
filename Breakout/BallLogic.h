#pragma once
#include <SDL3/SDL.h>

class Ball {
public:
    SDL_FRect rect;
    float vx, vy;
    bool attached;    // NEW: ball follows paddle until launch

    Ball(float size);

    void update(float dt);
    void bounceWalls(float screenW, float screenH);
    void bouncePaddle(const SDL_FRect& paddle);
    void checkOutOfBounds(float screenH, const SDL_FRect& paddle);
    void reset(float paddleX, float paddleY, float paddleW);

    void attachToPaddle(const SDL_FRect& paddle);
    void launch();
};
