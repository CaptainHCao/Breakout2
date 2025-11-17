#include <vector>
#include "BrickLogic.h"

// Spawn bricks in a grid in the top half of the screen
std::vector<Brick> spawnBricks(int screenWidth, int screenHeight, int brickW, int brickH, int margin, int padding) {
    std::vector<Brick> bricks;

    // Define the area for bricks: top half of the screen, leave margin
    int availableWidth = screenWidth - 2 * margin;
    int availableHeight = screenHeight / 2 - margin;

    int cols = availableWidth / (brickW + padding);
    int rows = availableHeight / (brickH + padding);

    float startX = margin;
    float startY = margin;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Brick b;
            b.rect.w = brickW;
            b.rect.h = brickH;
            b.rect.x = startX + c * (brickW + padding);
            b.rect.y = startY + r * (brickH + padding);
            bricks.push_back(b);
        }
    }

    return bricks;
}
