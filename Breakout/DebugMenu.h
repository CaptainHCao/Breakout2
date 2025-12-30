#pragma once

#include <vector>
#include "Paddle.h"
#include "Ball.h"
#include "UpgradePickup.h"

// Renders the debug menu.
// Returns true if the window is still open.
void renderDebugMenu(
    bool& showDebugMenu,
    Paddle& paddle,
    Ball& ball,
    std::vector<UpgradePickup>& pickups
);
