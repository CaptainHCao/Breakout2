#pragma once
#include <SDL3/SDL.h>

class Game;   // forward declaration

enum class GameStateID {
    Menu,
    Playing
};

class IGameState
{
public:
    virtual ~IGameState() = default;

    virtual void handleEvent(Game& game, const SDL_Event& e) = 0;
    virtual void update(Game& game, float dt) = 0;
    virtual void render(Game& game) = 0;
};

// ---- Startup screen (Menu) ----
class MenuState : public IGameState
{
public:
    void handleEvent(Game& game, const SDL_Event& e) override;
    void update(Game& game, float dt) override;
    void render(Game& game) override;
};

// ---- Actual gameplay ----
class PlayingState : public IGameState
{
public:
    void handleEvent(Game& game, const SDL_Event& e) override;
    void update(Game& game, float dt) override;
    void render(Game& game) override;
};
