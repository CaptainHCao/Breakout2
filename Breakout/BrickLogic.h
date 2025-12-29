#pragma once
#include <vector>
#include <string>
#include "Brick.h"
#include "Ball.h"
#include "UpgradeTypes.h"
#include "UpgradePickup.h"

struct UpgradePickup;

namespace BrickLogic {
    std::vector<Brick> spawnBricks(
        int screenWidth,
        int screenHeight,
        int brickW,
        int brickH,
        int margin,
        int padding,
        const std::string& mapsPath = "Maps.txt",
        int levelIndex = 0
    );

    void handleBallCollision(
        Ball& ball,
        std::vector<Brick>& bricks,
        std::vector<UpgradePickup>& pickups
    );

    void spawnPickupFromBrick(
        const Brick& brick,
        std::vector<UpgradePickup>& pickups
    );
}
