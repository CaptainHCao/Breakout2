#pragma once
#include <SDL3/SDL.h>

#include "SDLApp.h"
#include "Paddle.h"
#include "Menu.h"
#include "GameStates.h"
#include "Ball.h"
#include "Audio.h" 
#include "UpgradePickup.h"
#include <vector>

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
	int currentLevel = 0;

    bool startGame = false;
    bool quitFromMenu = false;
	bool debug = false;

    SDL_Texture* bgTexture = nullptr;

    const std::vector<UpgradePickup>& getPickups() const { return m_pickups; }
    std::vector<UpgradePickup>& getPickups() { return m_pickups; } // for spawning/removing

    GameStateID currentState;    // current state identifier

    Game();                      // ctor sets initial state
    int  run();
    void changeState(GameStateID newState);

private:
    MusicPlayer music;          // background music
    SoundEffect brickBreakSfx;  // sound when a brick breaks
    SoundEffect pickupSfx;      // for unpgrades

    std::vector<UpgradePickup> m_pickups;

};