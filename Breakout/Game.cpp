// main loopImplementation of the main game loop for the Breakout game.

#include "Game.h"

int Game::run() {
    if (!app.init()) return 1;
    if (!paddle.load(app.renderer)) return 1;

    const bool *keys = SDL_GetKeyboardState(nullptr);
    bool running = true;
    Uint64 prevTime = SDL_GetTicks();

    while (running) {
        Uint64 now = SDL_GetTicks();
        float dt = (now - prevTime) / 1000.0f;
        prevTime = now;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                app.width = e.window.data1;
                app.height = e.window.data2;
            }
        }

        paddle.update(dt, app.logicalWidth, keys);

        SDL_SetRenderDrawColor(app.renderer, 40, 40, 40, 255);
        SDL_RenderClear(app.renderer);

        paddle.render(app.renderer, app.logicalWidth);

        SDL_RenderPresent(app.renderer);
        SDL_Delay(16);
    }

    paddle.destroy();
    app.shutdown();
    return 0;
}
