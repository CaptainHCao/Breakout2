#pragma once
#include <SDL3/SDL.h>

#include "SDLApp.h"
#include "Paddle.h"
#include "Menu.h"

// forward declare enum, it is defined in GameStates.h
enum class GameStateID;

class Game {
public:
    SDLApp app;
    Paddle paddle;
    Menu menu;

    bool soundOn = true;
    bool musicOn = true;
    int  highscore = 0;

    bool startGame = false;
    bool quitFromMenu = false;

    SDL_Texture* bgTexture = nullptr;

    GameStateID currentState;    // current state identifier

    Game();                      // ctor sets initial state
    int  run();
    void changeState(GameStateID newState);
};
