#include <SDL3/SDL_main.h>
#include "Game.h"

static int main(int argc, char* argv[])
{
    Game game;
    return game.run();
}