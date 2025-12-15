#include "BrickLogic.h"

static bool overlaps(const SDL_FRect& a, const SDL_FRect& b)
{
    return a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y;
}

std::vector<Brick> BrickLogic::spawnBricks(
    int screenWidth,
    int screenHeight,
    int brickW,
    int brickH,
    int margin,
    int padding
) {
    std::vector<Brick> bricks;

    int availableWidth = screenWidth - 2 * margin;
    int availableHeight = screenHeight / 2 - margin;

    int cols = availableWidth / (brickW + padding);
    int rows = availableHeight / (brickH + padding);

    float startX = (float)margin;
    float startY = (float)margin;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Brick b;
            b.rect.w = (float)brickW;
            b.rect.h = (float)brickH;
            b.rect.x = startX + c * (brickW + padding);
            b.rect.y = startY + r * (brickH + padding);

            // default values if Brick has them:
            // b.alive = true;
            // b.hp = 1;

            bricks.push_back(b);
        }
    }
    return bricks;
}

void BrickLogic::handleBallCollision(
    Ball& ball,
    std::vector<Brick>& bricks,
    std::vector<UpgradePickup>& pickups
) {
    const SDL_FRect ballRect = ball.getRect(); 

    for (auto& brick : bricks)
    {
        if (!brick.alive) continue;

        if (overlaps(ballRect, brick.rect))
        {
            brick.alive = false;

            if (brick.hasUpgrade)
                BrickLogic::spawnPickupFromBrick(brick, pickups);

            if (!ball.isStrong()) {
                ball.bounceVertical();
            }
            else {
                ball.consumeStrongHit();
            }
            break;
        }
    }
}

void BrickLogic::spawnPickupFromBrick(
    const Brick& brick,
    std::vector<UpgradePickup>& pickups
) {
    UpgradePickup p;
    p.type = brick.upgradeType;
    p.rect = SDL_FRect{
        brick.rect.x + brick.rect.w * 0.25f,
        brick.rect.y + brick.rect.h * 0.25f,
        brick.rect.w * 0.5f,
        brick.rect.h * 0.5f
    };
    pickups.push_back(p);
}
