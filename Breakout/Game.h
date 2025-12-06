#pragma once
#include "SDLApp.h"
#include "Paddle.h"

class Game {
public:
    Game() = default;
    int run();   // main game loop

private:
    SDLApp app;
    Paddle paddle;
};
