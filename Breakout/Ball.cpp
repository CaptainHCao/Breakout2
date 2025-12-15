#include "Ball.h"
#include <cmath>

Ball::Ball(float size)
{
    rect.w = size;
    rect.h = size;
    rect.x = 0.0f;
    rect.y = 0.0f;

    vx = 0.0f;
    vy = 0.0f;
    attached = true;
}

void Ball::update(float dt)
{
    if (attached) return;

    rect.x += vx * dt;
    rect.y += vy * dt;
}

void Ball::attachToPaddle(const SDL_FRect& paddle)
{
    rect.x = paddle.x + paddle.w * 0.5f - rect.w * 0.5f;
    rect.y = paddle.y - rect.h - 1.0f;
    attached = true;
}

void Ball::launch()
{
    if (!attached) return;

    attached = false;
    vx = 120.0f;
    vy = -120.0f;
}

void Ball::bounceWalls(float screenW, float screenH)
{
    if (rect.x <= 0.0f || rect.x + rect.w >= screenW)
        vx = -vx;

    if (rect.y <= 0.0f)
        vy = -vy;
}

void Ball::bouncePaddle(const SDL_FRect& paddle)
{
    if (rect.y + rect.h < paddle.y) return;
    if (rect.x + rect.w < paddle.x || rect.x > paddle.x + paddle.w) return;

    vy = -std::abs(vy);

    float hitPos = (rect.x + rect.w * 0.5f) - paddle.x;
    float norm = (hitPos / paddle.w) - 0.5f;
    vx += norm * 60.0f;
}

void Ball::bounceVertical()
{
    vy = -vy;
}

bool Ball::checkOutOfBounds(float screenH, const SDL_FRect& paddle)
{
    if (rect.y > screenH)
    {
        attachToPaddle(paddle);
        return true;
    }
    return false;
}

void Ball::reset(float paddleX, float paddleY, float paddleW)
{
    rect.x = paddleX + paddleW * 0.5f - rect.w * 0.5f;
    rect.y = paddleY - rect.h - 1.0f;
    vx = 0.0f;
    vy = 0.0f;
    attached = true;
    strongHitsLeft = 0;
}

void Ball::setSizeKeepingCenter(float newSize)
{
    float cx = rect.x + rect.w * 0.5f;
    float cy = rect.y + rect.h * 0.5f;

    rect.w = newSize;
    rect.h = newSize;

    rect.x = cx - rect.w * 0.5f;
    rect.y = cy - rect.h * 0.5f;
}

void Ball::makeBigger(float factor)
{
    setSizeKeepingCenter(rect.w * factor);
}

void Ball::makeStrong(int hits)
{
    strongHitsLeft = hits;
}

void Ball::consumeStrongHit()
{
    if (strongHitsLeft > 0)
        strongHitsLeft--;
}
