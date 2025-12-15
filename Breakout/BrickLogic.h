#pragma once
#include <vector>

#include "Brick.h"
#include "Ball.h"
#include "UpgradePickup.h"

class BrickLogic
{
public:
    static std::vector<Brick> spawnBricks(
        int screenWidth,
        int screenHeight,
        int brickW,
        int brickH,
        int margin,
        int padding
    );

    static void handleBallCollision(
        Ball& ball,
        std::vector<Brick>& bricks,
        std::vector<UpgradePickup>& pickups
    );

    static void spawnPickupFromBrick(
        const Brick& brick,
        std::vector<UpgradePickup>& pickups
    );
};