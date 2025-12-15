#pragma once
#include <SDL3/SDL.h>

class Ball {
public:
    explicit Ball(float size);

    // --- Update ---
    void update(float dt);

    // --- State ---
    bool isAttached() const { return attached; }
    const SDL_FRect& getRect() const { return rect; }

    // --- Control ---
    void attachToPaddle(const SDL_FRect& paddle);
    void launch();
    void reset(float paddleX, float paddleY, float paddleW);

    // --- Collisions ---
    void bounceWalls(float screenW, float screenH);
    void bouncePaddle(const SDL_FRect& paddle);
    void bounceVertical();

    // --- Bounds ---
    bool checkOutOfBounds(float screenH, const SDL_FRect& paddle);

    // --- Upgrades ---
    void makeBigger(float factor);
    void makeStrong(int hits);
    bool isStrong() const { return strongHitsLeft > 0; }
    void consumeStrongHit();

private:
    void setSizeKeepingCenter(float newSize);

private:
    SDL_FRect rect{};
    float vx = 0.0f;
    float vy = 0.0f;
    bool attached = true;

    int strongHitsLeft = 0;
};
