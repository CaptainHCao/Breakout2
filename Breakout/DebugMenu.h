#pragma once

#include "Paddle.h"
#include "Ball.h"

// Renders the debug menu.
// Returns true if the window is still open.
void renderDebugMenu(
    bool& showDebugMenu,
    Paddle& paddle,
    Ball& ball
);
