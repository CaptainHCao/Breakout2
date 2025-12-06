// Game.h
#pragma once
#include <SDL3/SDL.h>
#include "SDLApp.h"
#include "Paddle.h"

struct Game {
    SDLApp app;
    Paddle paddle;      // later you add Ball, Bricks, Audio, Menu, etc.

    int run();
};

IGameState* currentState = nullptr;
MenuState menuState;
PlayingState playingState;

void changeState(GameStateID id);