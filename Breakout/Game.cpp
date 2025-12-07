#include "Game.h"
#include "Menu.h"
#include "SDLApp.h"
#include "GameStates.h"  

#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"


Game::Game()
    : currentState(GameStateID::Menu)    
{
}


int Game::run()
{
    // --- Init SDL/window/renderer ---
    app.width = 1600;
    app.height = 900;
    app.logicalWidth = 640;
    app.logicalHeight = 320;

    if (!app.init("Breakout Game"))
        return 1;

    if (!app.music.init("assets/funarcade.wav"))
        SDL_Log("Music init failed!");

    // --- Init ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(app.window, app.renderer);
    ImGui_ImplSDLRenderer3_Init(app.renderer);

    // --- Load assets ---
    SDL_Texture* paddleTexture = IMG_LoadTexture(app.renderer, "assets/paddle.png");
    if (!paddleTexture) {
        SDL_Log("Failed to load paddle texture: %s", SDL_GetError());
        app.shutdown();
        return 1;
    }

    SDL_Texture* bgTexture = IMG_LoadTexture(app.renderer, "assets/dragon.png");
    if (!bgTexture) {
        SDL_Log("Failed to load dragon.png: %s", SDL_GetError());
    }

    SDL_SetTextureScaleMode(bgTexture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(paddleTexture, SDL_SCALEMODE_NEAREST);

    const float spriteSize = 32.0f;
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float paddleX = 0.0f;
    float paddleY = 280.0f;

    // --- Game / menu state ---
    enum class GameState { Menu, Playing };
    GameState gameState = GameState::Menu;   // ? STARTUP SCREEN FIRST

    bool soundOn = true;
    bool musicOn = true;
    int  highscore = 0;

    Menu menu;

    bool running = true;
    uint64_t prevTime = SDL_GetTicks();

    SDL_Log("DEBUG: Entering game loop, initial state = MENU");

    // ===========================
    //         GAME LOOP
    // ===========================
    while (running)
    {
        // --- Time step ---
        const uint64_t nowTime = SDL_GetTicks();
        const float    deltaTime = (nowTime - prevTime) / 1000.0f;
        prevTime = nowTime;

        // Per–frame menu actions (set by keyboard and ImGui)
        bool startGame = false;
        bool quitFromMenu = false;

        // --- Event handling ---
        SDL_Event event{ 0 };
        while (SDL_PollEvent(&event))
        {
            // Let ImGui consume all events first
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT) {
                SDL_Log("DEBUG: SDL_EVENT_QUIT received");
                running = false;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                app.width = event.window.data1;
                app.height = event.window.data2;
                SDL_Log("DEBUG: Window resized to %d x %d", app.width, app.height);
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Log("DEBUG: Key down: scancode=%d", event.key.scancode);

                if (gameState == GameState::Menu) {
                    // keyboard controls for the startup screen
                    menu.handleEvent(
                        event,
                        soundOn, musicOn, highscore,
                        startGame, quitFromMenu
                    );
                }
                else if (gameState == GameState::Playing) {
                    // ESC to go back to startup screen
                    if (event.key.key == SDLK_ESCAPE) {
                        SDL_Log("DEBUG: ESC in Playing -> go back to Menu");
                        gameState = GameState::Menu;
                    }
                }
            }
        }

        // --- Start ImGui frame ---
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        {
            float scaleX, scaleY;
            SDL_GetRenderScale(app.renderer, &scaleX, &scaleY);
            ImGuiIO& io = ImGui::GetIO();
            io.DisplayFramebufferScale = ImVec2(scaleX, scaleY);
        }

        // ?? DEBUG OVERLAY: always show current state in a small ImGui window
        {
            ImGui::Begin("DEBUG STATE");
            ImGui::Text("State: %s",
                (gameState == GameState::Menu) ? "MENU (startup)" : "PLAYING");
            ImGui::End();
        }

        // --- Game update (only when playing) ---
        if (gameState == GameState::Playing)
        {
            const float paddleSpeed = 150.0f; // pixels per second
            float moveAmount = 0.0f;

            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
                moveAmount -= paddleSpeed;
            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
                moveAmount += paddleSpeed;

            paddleX += moveAmount * deltaTime;

            // Wrap paddleX within logical screen bounds
            if (paddleX < -spriteSize)
                paddleX = app.logicalWidth - spriteSize;
            else if (paddleX > app.logicalWidth)
                paddleX = 0.0f;
        }

        // --- Rendering ---
        SDL_SetRenderDrawColor(app.renderer, 10, 10, 40, 255);
        SDL_RenderClear(app.renderer);

        if (gameState == GameState::Menu)
        {
            SDL_Log("DEBUG: Rendering MENU frame");

            // Background
            SDL_FRect bgRect{
                .x = 0.0f,
                .y = 0.0f,
                .w = (float)app.logicalWidth,
                .h = (float)app.logicalHeight
            };
            if (bgTexture) {
                SDL_RenderTexture(app.renderer, bgTexture, nullptr, &bgRect);
            }

            // Startup screen UI on top
            menu.render(
                app.renderer,
                soundOn, musicOn, highscore,
                startGame, quitFromMenu,
                app.logicalWidth, app.logicalHeight
            );
        }
        else if (gameState == GameState::Playing)
        {
            SDL_Log("DEBUG: Rendering PLAYING frame, paddleX=%.2f", paddleX);

            SDL_FRect src{ 0.0f, 0.0f, spriteSize, spriteSize };
            SDL_FRect dst{ paddleX, paddleY, spriteSize, spriteSize };

            SDL_RenderTexture(app.renderer, paddleTexture, &src, &dst);

            // Optional wrap–around rendering
            if (paddleX < spriteSize)
            {
                SDL_FRect wrapDst = dst;
                wrapDst.x = paddleX + app.logicalWidth;
                SDL_RenderTexture(app.renderer, paddleTexture, &src, &wrapDst);
            }
            else if (paddleX + spriteSize > app.logicalWidth - spriteSize)
            {
                SDL_FRect wrapDst = dst;
                wrapDst.x = paddleX - app.logicalWidth;
                SDL_RenderTexture(app.renderer, paddleTexture, &src, &wrapDst);
            }
        }

        // --- React to menu actions (keyboard or ImGui) ---
        if (gameState == GameState::Menu)
        {
            if (quitFromMenu) {
                SDL_Log("DEBUG: quitFromMenu = true -> exiting");
                running = false;
            }
            else if (startGame) {
                SDL_Log("DEBUG: startGame = true -> switch to PLAYING");
                gameState = GameState::Playing;
                paddleX = 0.0f;
                paddleY = 280.0f;
            }
        }

        // --- ImGui + present + music ---
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), app.renderer);

        app.music.setEnabled(musicOn);
        app.music.update();

        SDL_RenderPresent(app.renderer);

        // Simple frame cap (~60 FPS)
        SDL_Delay(16);
    }

    // Cleanup (after the loop)
    SDL_DestroyTexture(paddleTexture);
    if (bgTexture) SDL_DestroyTexture(bgTexture);

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    app.shutdown();
    return 0;

}

void Game::changeState(GameStateID newState) {
    currentState = newState;
}
