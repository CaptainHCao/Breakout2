#include "Game.h"
#include "Menu.h"
#include "SDLApp.h"
#include "GameStates.h"

#include "Paddle.h"
#include "Brick.h"
#include "BrickLogic.h"
#include "BallLogic.h"

#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

static bool overlaps(const SDL_FRect& a, const SDL_FRect& b)
{
    return a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y;
}

Game::Game()
    : currentState(GameStateID::Menu)
{
}

int Game::run()
{
    int  highscore = 0;
    int lives = 3;
	int score = 0;
    
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

    if (!paddle.load(app.renderer)) {
        app.shutdown();
        return 1;
    }

    SDL_Texture* bgTexture = IMG_LoadTexture(app.renderer, "assets/dragon.png");
    if (!bgTexture) {
        SDL_Log("Failed to load dragon.png: %s", SDL_GetError());
    }
    SDL_SetTextureScaleMode(bgTexture, SDL_SCALEMODE_NEAREST);


    SDL_Texture* ballTexture = IMG_LoadTexture(app.renderer, "assets/Ball.png");
    if (!ballTexture) {
        SDL_Log("Failed to load Ball.png: %s", SDL_GetError());
    }
    SDL_SetTextureScaleMode(ballTexture, SDL_SCALEMODE_NEAREST);
    Ball ball(8.0f);

    SDL_Texture* brickTexture = IMG_LoadTexture(app.renderer, "assets/paddle.png");
    if (!brickTexture) {
        SDL_Log("Failed to load brick texture: %s", SDL_GetError());
    }

    //--- Create bricks ---
    const int brickW = 32;
    const int brickH = 16;
    const int margin = 10;   // distance from edges
    const int padding = 4;    // gap between bricks

    std::vector<Brick> bricks = spawnBricks(
        app.logicalWidth,
        app.logicalHeight,
        brickW,
        brickH,
        margin,
        padding
    );

    // assign texture to each brick
    for (auto& b : bricks) {
        b.texture = brickTexture;
    }


    // --- Game / menu state ---
    enum class GameState { Menu, Playing };
    GameState gameState = GameState::Menu;   // STARTUP SCREEN FIRST

    bool soundOn = true;
    bool musicOn = true;
    

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
                    else if (event.key.key == SDLK_SPACE) {
                        ball.launch();  
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

        //DEBUG OVERLAY: always show current state in a small ImGui window
        //{
        //    ImGui::Begin("DEBUG STATE");
        //    ImGui::Text("State: %s",
        //        (gameState == GameState::Menu) ? "MENU (startup)" : "PLAYING");
        //    ImGui::End();
        //}

        // --- Game update (only when playing) ---
        if (gameState == GameState::Playing)
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.35f); // slightly transparent

            ImGui::Begin("HUD",
                nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav);

            ImGui::Text("Lives: %d", lives);
            ImGui::Text("Score: %d", score);
			ImGui::Text("Highscore: %d", highscore);

            ImGui::End();

            paddle.update(deltaTime, app.logicalWidth);
            SDL_FRect paddleRect = paddle.getRect();

            // keep ball attached to paddle if not launched yet
            if (ball.isAttached()) {
                ball.attachToPaddle(paddleRect);
            }

            ball.update(deltaTime);
            ball.bounceWalls((float)app.logicalWidth, (float)app.logicalHeight);
            ball.bouncePaddle(paddleRect);
            if (ball.checkOutOfBounds((float)app.logicalHeight, paddleRect)) {
                lives--;

                if (lives <= 0) {

                    // Update highscore
                    if (score > highscore) {
                        highscore = score;
					}
                    // out of lives -> go back to menu (and maybe reset)
                    SDL_Log("DEBUG: no lives left -> back to MENU");
                    gameState = GameState::Menu;

                    // simple reset: restore lives & score
                    lives = 3;
                    score = 0;

                    // (optional) respawn bricks here using spawnBricks again
                }
			}

			// check ball-brick collisions
            SDL_FRect ballRect = ball.getRect();

            for (auto& brick : bricks) {
                if (!brick.alive) continue;

                if (overlaps(ballRect, brick.rect)) {
                    // remove the brick
                    brick.alive = false;
                    score += 10;
                    // simple bounce: flip vertical velocity
                    ball.bounceVertical();
                    // handle only one brick per frame
                    break;
                }
            }

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
            for (const auto& brick : bricks) {
                brick.render(app.renderer);
            }

            paddle.render(app.renderer, app.logicalWidth);
            if (ballTexture) {
                SDL_FRect ballRect = ball.getRect();
                SDL_RenderTexture(app.renderer, ballTexture, nullptr, &ballRect);
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
				paddle.setPosition(0.0f, 280.0f);
                SDL_FRect pRect = paddle.getRect();
                ball.attachToPaddle(pRect);
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
	paddle.destroy();

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
