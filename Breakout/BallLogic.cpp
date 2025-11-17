#include "BallLogic.h"
#include <cmath>
#include <cstdlib>

Ball::Ball(float size) {
    rect = { 0,0,size,size };
    vx = 0;
    vy = 0;
    attached = true;    // start attached to paddle
}

void Ball::update(float dt) {
    if (!attached) {
        rect.x += vx * dt;
        rect.y += vy * dt;
    }
}

void Ball::attachToPaddle(const SDL_FRect& paddle) {
    // Center the ball horizontally on paddle
    rect.x = paddle.x + paddle.w / 2.0f - rect.w / 2.0f;
    rect.y = paddle.y - rect.h;
}

void Ball::launch() {
    if (!attached) return;

    attached = false;

    // Random angle between -60° and +60° (to left or right)
    float minAngle = -60.0f * (3.14159f / 180.0f);
    float maxAngle = 60.0f * (3.14159f / 180.0f);

    float angle = minAngle + static_cast<float>(rand()) / RAND_MAX * (maxAngle - minAngle);

    float speed = 160.0f; // your speed

    vx = speed * sinf(angle);
    vy = -speed * cosf(angle);  // always upward
}

void Ball::bounceWalls(float screenW, float screenH) {
    if (rect.x < 0) { rect.x = 0; vx = -vx; }
    else if (rect.x + rect.w > screenW) { rect.x = screenW - rect.w; vx = -vx; }

    if (rect.y < 0) { rect.y = 0; vy = -vy; }
}
void Ball::checkOutOfBounds(float screenH, const SDL_FRect& paddle) {
    if (rect.y > screenH) {   // ball fell below screen
        attached = true;      // reattach to paddle
        vx = 0;
        vy = 0;
        attachToPaddle(paddle);  // position it on the paddle
    }
}
void Ball::reset(float paddleX, float paddleY, float paddleW) {
    attached = true;
    rect.x = paddleX + paddleW / 2 - rect.w / 2;
    rect.y = paddleY - rect.h;
    vx = 0;
    vy = 0;
}
void Ball::bouncePaddle(const SDL_FRect& paddle) {
    if (attached) return;

    bool overlapX = 
        rect.x < paddle.x + paddle.w &&
        rect.x + rect.w > paddle.x;
    bool overlapY =
        rect.y + rect.h >= paddle.y &&
        rect.y + rect.h <= paddle.y + paddle.h + 2;

    if (!(overlapX && overlapY)) return;

    rect.y = paddle.y - rect.h;
    vy = -fabs(vy);

    float paddleCenter = paddle.x + paddle.w / 2.0f;
    float ballCenter = rect.x + rect.w / 2.0f;
    float distance = (ballCenter - paddleCenter);
    float normalized = distance / (paddle.w / 2.0f);

    float maxAngle = 60.0f * (3.14159f / 180.0f);
    float angle = normalized * maxAngle;

    float speed = sqrt(vx * vx + vy * vy);

    vx = speed * sin(angle);
    vy = -speed * cos(angle);
}