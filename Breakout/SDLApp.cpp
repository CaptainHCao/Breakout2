// SDLApp.cpp
#include "SDLApp.h"

bool SDLApp::init(const char* title)
{
    // Init SDL3 (video + audio)
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Error",
            "Failed to init SDL3 (video+audio)",
            nullptr
        );
        return false;
    }

    // Create window
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Error",
            "Failed to create window",
            nullptr
        );
        shutdown();
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Error",
            "Failed to create renderer",
            window
        );
        shutdown();
        return false;
    }

    SDL_SetRenderLogicalPresentation(
        renderer,
        logicalWidth,
        logicalHeight,
        SDL_LOGICAL_PRESENTATION_LETTERBOX
    );

    return true;
}

void SDLApp::shutdown()
{
    music.shutdown();  

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}
