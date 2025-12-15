#include "Game.h"
#include "Menu.h"
#include "SDLApp.h"
#include "GameStates.h"
#include "Paddle.h"
#include "Brick.h"
#include "BrickLogic.h"
#include "Ball.h"

#include "DebugMenu.h"

#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <algorithm>

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

    if (!brickBreakSfx.load("assets/brick_break.wav")) {
        SDL_Log("Brick SFX init failed!");
    }

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

    std::vector<Brick> bricks = BrickLogic::spawnBricks(
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


    std::vector<UpgradePickup> pickups;


    // --- Game / menu state ---
    enum class GameState { Menu, Playing };
    GameState gameState = GameState::Menu;   // STARTUP SCREEN FIRST

    bool soundOn = true;
    bool musicOn = true;
    bool showDebugMenu = true;

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
            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                float rx, ry;
                SDL_RenderCoordinatesFromWindow(app.renderer, event.motion.x, event.motion.y, &rx, &ry);
                event.motion.x = rx;
                event.motion.y = ry;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                float rx, ry;
                SDL_RenderCoordinatesFromWindow(app.renderer, event.button.x, event.button.y, &rx, &ry);
                event.button.x = rx;
                event.button.y = ry;
            }

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
                    else if (event.key.key == SDLK_M) {
                        musicOn = !musicOn;
                        SDL_Log("Music toggled %s", musicOn ? "ON" : "OFF");
                    }
                    else if (event.key.key == SDLK_O) {
                        soundOn = !soundOn;
                        SDL_Log("Sound toggled %s", soundOn ? "ON" : "OFF");
                    }
                    else if (event.key.key == SDLK_F1) { 
                        showDebugMenu = !showDebugMenu; 
                    }
                }
            }
        }

        // --- Start ImGui frame ---
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        renderDebugMenu(showDebugMenu, paddle, ball);

        {
            float scaleX, scaleY;
            SDL_GetRenderScale(app.renderer, &scaleX, &scaleY);
            ImGuiIO& io = ImGui::GetIO();
            io.DisplayFramebufferScale = ImVec2(scaleX, scaleY);
        }

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

            // --- update + collect falling upgrades ---
            for (auto& p : pickups)
            {
                if (!p.alive) continue;

                p.update(deltaTime);

                // collected by paddle
                if (overlaps(p.rect, paddleRect))
                {
                    switch (p.type)
                    {
                    case UpgradeType::BiggerBall:
                        ball.makeBigger(1.25f);
                        break;

                    case UpgradeType::StrongerBall:
                        ball.makeStrong(5);
                        break;

                    case UpgradeType::BiggerPaddle:
                        paddle.makeBigger(1.25f);   // add this in Paddle
                        break;

                    case UpgradeType::FasterPaddle:
                        paddle.makeFaster(1.20f);   // add this in Paddle
                        break;
                    }

                    p.alive = false;
                }

                // fell off screen
                if (p.rect.y > app.logicalHeight) p.alive = false;
            }

            // cleanup dead pickups
            pickups.erase(
                std::remove_if(pickups.begin(), pickups.end(),
                    [](const UpgradePickup& p) { return !p.alive; }),
                pickups.end()
            );


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

                    if (brick.hasUpgrade)
                    {
                        UpgradePickup p;
                        p.type = brick.upgradeType;
                        p.rect = SDL_FRect{
                            brick.rect.x + brick.rect.w * 0.25f,
                            brick.rect.y + brick.rect.h * 0.25f,
                            brick.rect.w * 0.5f,
                            brick.rect.h * 0.5f
                        };
                        pickups.push_back(p);
                    }

                    if (soundOn) {
                        brickBreakSfx.play();
                    }
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

            for (const auto& p : pickups) {
                p.render(app.renderer);
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
				// paddle spawns in the middle bottom
                float startX = (app.logicalWidth - paddle.getRect().w) * 0.5f;
                float startY = app.logicalHeight - 40;  
                paddle.setPosition(startX, startY);
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
    brickBreakSfx.shutdown();

    app.shutdown();
    return 0;
    
}   

void Game::changeState(GameStateID newState) {
    currentState = newState;
}
