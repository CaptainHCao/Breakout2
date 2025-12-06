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
};