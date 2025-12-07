// BrickLogic.h
#pragma once
#include <vector>
#include "Brick.h"

std::vector<Brick> spawnBricks(
    int screenWidth,
    int screenHeight,
    int brickW,
    int brickH,
    int margin,
    int padding
);
