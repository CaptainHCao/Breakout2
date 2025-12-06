// Window + Renderer management

#include "SDLApp.h"
#include <SDL3_image/SDL_image.h>

bool SDLApp::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    window = SDL_CreateWindow("Breakout", width, height, SDL_WINDOW_RESIZABLE);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) return false;

    SDL_SetRenderLogicalPresentation(
        renderer, logicalWidth, logicalHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX
    );

    return true;
}

void SDLApp::shutdown() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
