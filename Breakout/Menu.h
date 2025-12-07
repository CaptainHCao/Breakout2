// Menu.h
#pragma once

#include <SDL3/SDL.h>

class Menu
{
public:
    void render(SDL_Renderer *renderer,
                bool soundOn, bool musicOn, int highscore,
                bool &startGame, bool &quit,
                int logicalWidth, int logicalHeight);

    void handleEvent(const SDL_Event &event,
                     bool &soundOn, bool &musicOn, int &highscore,
                     bool &startGame, bool &quit);
    SDL_FRect getRect() const;

private:
    float x = 0.0f;
    float y = 280.0f;
    float size = 32.0f;
    float speed = 160.0f;
    SDL_Texture* texture = nullptr;
};