// main loopImplementation of the main game loop for the Breakout game.

#include "Game.h"
#include "Menu.h"
#include "SDLApp.h"

#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"



void Game::changeState(GameStateID id)
{
    switch (id)
    {
    case GameStateID::Menu:
        SDL_Log("DEBUG: Switching to MENU");
        currentState = &menuState;
        break;

    case GameStateID::Playing:
        SDL_Log("DEBUG: Switching to PLAYING");
        currentState = &playingState;
        break;
    }
}

int Game::run()
{
    // --- Init ---
    app.width = 1600;
    app.height = 900;
    app.logicalWidth = 640;
    app.logicalHeight = 320;

    if (!app.init("Breakout Game"))
        return 1;

    app.music.init("assets/audio/funarcade.wav");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(app.window, app.renderer);
    ImGui_ImplSDLRenderer3_Init(app.renderer);

    // --- Load assets ---
    paddleTexture = IMG_LoadTexture(app.renderer, "assets/textures/paddle.png");
    bgTexture = IMG_LoadTexture(app.renderer, "assets/textures/dragon.png");
    SDL_SetTextureScaleMode(bgTexture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(paddleTexture, SDL_SCALEMODE_NEAREST);

    // --- Game state ---
    enum class GameState { Menu, Playing };
    GameState gameState = GameState::Menu;

    Menu menu;

    bool soundOn = true;
    bool musicOn = true;
    int  highscore = 0;

    const bool* keys = SDL_GetKeyboardState(NULL);

    float paddleX = 0.0f;
    float paddleY = 280.0f;
    const float spriteSize = 32.0f;

    bool running = true;
    uint64_t prevTime = SDL_GetTicks();

    // -------------------------
    //        MAIN LOOP
    // -------------------------
    while (running)
    {
        // --- Time step ---
        uint64_t now = SDL_GetTicks();
        float dt = (now - prevTime) / 1000.0f;
        prevTime = now;

        // These will be set by Menu
        bool startGame = false;
        bool quitFromMenu = false;

        // -------- EVENTS --------
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                app.width = event.window.data1;
                app.height = event.window.data2;
            }

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (gameState == GameState::Menu)
                {
                    menu.handleEvent(event, soundOn, musicOn,
                        highscore, startGame, quitFromMenu);
                }
                else if (gameState == GameState::Playing)
                {
                    if (event.key.key == SDLK_ESCAPE)
                        gameState = GameState::Menu;
                }
            }
        }

        // -------- IMGUI BEGIN --------
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // -------- UPDATE --------
        if (gameState == GameState::Playing)
        {
            float paddleSpeed = 150.0f;
            float dx = 0.0f;

            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  dx -= paddleSpeed;
            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) dx += paddleSpeed;

            paddleX += dx * dt;

            // wrap-around
            if (paddleX < -spriteSize)
                paddleX = app.logicalWidth - spriteSize;
            else if (paddleX > app.logicalWidth)
                paddleX = 0.0f;
        }

        // -------- RENDER --------
        SDL_SetRenderDrawColor(app.renderer, 10, 10, 40, 255);
        SDL_RenderClear(app.renderer);

        if (gameState == GameState::Menu)
        {
            // Background
            SDL_FRect bgRect{ 0,0,(float)app.logicalWidth,(float)app.logicalHeight };
            SDL_RenderTexture(app.renderer, bgTexture, NULL, &bgRect);

            // Menu UI
            menu.render(app.renderer, soundOn, musicOn, highscore,
                startGame, quitFromMenu,
                app.logicalWidth, app.logicalHeight);
        }
        else  // PLAYING
        {
            SDL_FRect src{ 0,0,spriteSize,spriteSize };
            SDL_FRect dst{ paddleX,paddleY,spriteSize,spriteSize };
            SDL_RenderTexture(app.renderer, paddleTexture, &src, &dst);
        }

        // -------- STATE SWITCH --------
        if (gameState == GameState::Menu)
        {
            if (quitFromMenu)
                running = false;

            if (startGame)
            {
                gameState = GameState::Playing;
                paddleX = 0;
                paddleY = 280;
            }
        }

        // -------- IMGUI END + PRESENT --------
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), app.renderer);

        app.music.setEnabled(musicOn);
        app.music.update();

        SDL_RenderPresent(app.renderer);

        // 60 FPS cap
        SDL_Delay(16);
    }

    // --- Cleanup ---
    SDL_DestroyTexture(paddleTexture);
    SDL_DestroyTexture(bgTexture);

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    app.shutdown();
    return 0;
}

