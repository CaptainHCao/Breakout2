#include "GameStates.h"
#include "Game.h"
#include "Menu.h"
#include "Paddle.h"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

// ---------------------- MENU STATE ----------------------

void MenuState::handleEvent(Game& game, const SDL_Event& e)
{
    game.menu.handleEvent(
        e,
        game.soundOn,
        game.musicOn,
        game.highscore,
        game.startGame,
        game.quitFromMenu
    );

    if (game.startGame)
        game.changeState(GameStateID::Playing);
}

void MenuState::update(Game& game, float dt)
{
    // Nothing needed for now
}

void MenuState::render(Game& game)
{
    SDL_RenderClear(game.app.renderer);

    // Draw background
    SDL_FRect rect{ 0,0,(float)game.app.logicalWidth,(float)game.app.logicalHeight };
    SDL_RenderTexture(game.app.renderer, game.bgTexture, nullptr, &rect);

    // Draw menu UI (ImGui)
    game.menu.render(
        game.app.renderer,
        game.soundOn,
        game.musicOn,
        game.highscore,
        game.startGame,
        game.quitFromMenu,
        game.app.logicalWidth,
        game.app.logicalHeight
    );
}

// ---------------------- PLAYING STATE ----------------------

void PlayingState::handleEvent(Game& game, const SDL_Event& e)
{
    if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE)
        game.changeState(GameStateID::Menu);
}

void PlayingState::update(Game& game, float dt)
{
    game.paddle.update(dt, game.app.logicalWidth, game.keys);
}

void PlayingState::render(Game& game)
{
    SDL_SetRenderDrawColor(game.app.renderer, 10, 10, 40, 255);
    SDL_RenderClear(game.app.renderer);

    game.paddle.render(game.app.renderer, game.app.logicalWidth);
}
