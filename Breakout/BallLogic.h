#pragma once
#include <SDL3/SDL.h>

class Ball {
public:
    Ball(float size);
    void update(float dt);
    void attachToPaddle(const SDL_FRect& paddle);
    void launch();
    void bounceWalls(float screenW, float screenH);
    void bouncePaddle(const SDL_FRect& paddle);
    void checkOutOfBounds(float screenH, const SDL_FRect& paddle);
    void reset(float paddleX, float paddleY, float paddleW);

    bool isAttached() const { return attached; }          
    const SDL_FRect& getRect() const { return rect; }

private:
    SDL_FRect rect;
    float vx, vy;
    bool attached;
};
