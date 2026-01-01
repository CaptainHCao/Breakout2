#include "Game.h"
#include "Menu.h"
#include "SDLApp.h"
#include "GameStates.h"
#include "Paddle.h"
#include "Brick.h"
#include "BrickLogic.h"
#include "Ball.h"
#include "UpgradePickup.h"  

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
    if (!pickupSfx.load("assets/pickup.wav")) {
        SDL_Log("Pickup SFX init failed!");
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

	// --- Load textures ---
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

    SDL_Texture* brickTexture = IMG_LoadTexture(app.renderer, "assets/bricks.png");
    if (!brickTexture) {
        SDL_Log("Failed to load brick texture: %s", SDL_GetError());
    }

    SDL_Texture* upgradeTexture = IMG_LoadTexture(app.renderer, "assets/upgradeOrb.png");
    if (!upgradeTexture) {
        SDL_Log("Failed to load upgrade.png: %s", SDL_GetError());
    }
    SDL_SetTextureScaleMode(upgradeTexture, SDL_SCALEMODE_NEAREST);

    //--- Create bricks ---
    const int brickW = 32;
    const int brickH = 16;
    const int margin = 10;   // distance from edges
    const int padding = 4;    // gap between bricks

    std::vector<Brick> bricks;
    try {
        bricks = BrickLogic::spawnBricks(
            app.logicalWidth,
            app.logicalHeight,
            brickW,
            brickH,
            margin,
            padding
        );
    } catch (const std::exception& e) {
        SDL_Log("spawnBricks failed: %s", e.what());
        bricks.clear();
    }

    // setting up the randomness of upgrades
    srand((unsigned)SDL_GetTicks());

    int currentLevel = 0;
    constexpr int MAX_LEVELS = 4;
    int bricksRemaining = 0;

    auto loadLevel = [&](int levelIdx)
        {
            m_pickups.clear();

            bricks.clear();
            try {
                bricks = BrickLogic::spawnBricks(
                    app.logicalWidth, app.logicalHeight,
                    brickW, brickH,
                    margin, padding,
                    "Maps.txt",
                    levelIdx
                );
            }
            catch (const std::exception& e) {
                SDL_Log("spawnBricks failed: %s", e.what());
                bricks.clear();
            }

            for (auto& b : bricks) b.texture = brickTexture;

            bricksRemaining = 0;
            for (const auto& b : bricks) if (b.alive) bricksRemaining++;

            // reset paddle + ball for new level
            float startX = (app.logicalWidth - paddle.getRect().w) * 0.5f;
            float startY = app.logicalHeight - 40.0f;
            paddle.setPosition(startX, startY);
            ball.attachToPaddle(paddle.getRect());
        };

    loadLevel(currentLevel);

    // --- Game / menu state ---
    enum class GameState { Menu, Playing, Victory };
    GameState gameState = GameState::Menu;   // STARTUP SCREEN FIRST

    bool soundOn = true;
    bool musicOn = true;
    bool showDebugMenu = false;

    Menu menu;

    bool running = true;
    uint64_t prevTime = SDL_GetTicks();

    if (debug) {SDL_Log("DEBUG: Entering game loop, initial state = MENU");};

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
                if (debug) {SDL_Log("DEBUG: SDL_EVENT_QUIT received");}
                
                running = false;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                app.width = event.window.data1;
                app.height = event.window.data2;
                if (debug) {SDL_Log("DEBUG: Window resized to %d x %d", app.width, app.height);}
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (debug) {SDL_Log("DEBUG: Key down: scancode=%d", event.key.scancode);}

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
                        if (debug) {SDL_Log("DEBUG: ESC in Playing -> go back to Menu");}
                        gameState = GameState::Menu;
                        brickBreakSfx.stop();
                        pickupSfx.stop();
                    }
                    else if (event.key.key == SDLK_SPACE) {
                        ball.launch();  
                    }
                    else if (event.key.key == SDLK_M) {
                        musicOn = !musicOn;
                        if (debug) {SDL_Log("Music toggled %s", musicOn ? "ON" : "OFF"); }
                    }
                    else if (event.key.key == SDLK_O) {
                        soundOn = !soundOn;
                        if (debug) {SDL_Log("Sound toggled %s", soundOn ? "ON" : "OFF"); }
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

        renderDebugMenu(showDebugMenu, paddle, ball, m_pickups);

        {
            float scaleX, scaleY;
            SDL_GetRenderScale(app.renderer, &scaleX, &scaleY);
            ImGuiIO& io = ImGui::GetIO();
            io.DisplayFramebufferScale = ImVec2(scaleX, scaleY);
        }

        // --- Game update (only when playing) ---
        if (gameState == GameState::Playing)
        {
            const float hudHeight = 22.0f;

            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2((float)app.logicalWidth, hudHeight), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.6f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
            ImGui::Begin("HUD",
                nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav);

            ImGui::Columns(4, nullptr, false);

            ImGui::Text("Lives: %d", lives);        ImGui::NextColumn();
            ImGui::Text("Score: %d", score);        ImGui::NextColumn();
            ImGui::Text("Highscore: %d", highscore); ImGui::NextColumn();
            ImGui::Text("Level: %d", currentLevel + 1);

            ImGui::Columns(1);

            ImGui::End();
            ImGui::PopStyleVar();

            static float brickSfxCooldown = 0.0f;
            brickSfxCooldown -= deltaTime;

            paddle.update(deltaTime, app.logicalWidth);
            SDL_FRect paddleRect = paddle.getRect();

            // --- update + collect falling upgrades ---
            for (auto& p : m_pickups)
            {
                if (!p.alive) continue;

                p.update(deltaTime);

                // collected by paddle
                if (overlaps(p.rect, paddleRect))
                {
                    if (soundOn) {
                        pickupSfx.play();   
                    }
                    switch (p.type)
                    {
                    case UpgradeType::BiggerBall:
                        ball.makeBigger(1.25f);
                        break;

                    case UpgradeType::StrongerBall:
                        ball.makeStrong(5);
                        break;

                    case UpgradeType::BiggerPaddle:
                        paddle.makeBigger(1.25f);   
                        break;

                    case UpgradeType::FasterPaddle:
                        paddle.makeFaster(1.20f);   
                        break;
                    }

                    p.alive = false;
                }

                // fell off screen
                if (p.rect.y > app.logicalHeight) p.alive = false;
            }

            // cleanup dead pickups
            m_pickups.erase(
                std::remove_if(m_pickups.begin(), m_pickups.end(),
                    [](const UpgradePickup& p) { return !p.alive; }),
                m_pickups.end()
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
                    if (debug) {SDL_Log("DEBUG: no lives left -> back to MENU");}
                    gameState = GameState::Menu;
                    brickBreakSfx.stop();
                    pickupSfx.stop();

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
                    brick.hit();

                    // score only when destroyed
                    if (!brick.alive) {
                        score += 10 * brick.maxState;
                        bricksRemaining--;

                        if (brick.hasUpgrade) {
                            UpgradePickup p;
                            p.type = brick.upgradeType;
                            p.texture = upgradeTexture;
                            p.alive = true;

                            p.initAnimation();

                            // size = one frame, not whole texture
                            float fw = (float)p.frameW;
                            float fh = (float)p.frameH;

                            p.rect = SDL_FRect{
                                brick.rect.x + (brick.rect.w - fw) * 0.5f,
                                brick.rect.y + (brick.rect.h - fh) * 0.5f,
                                fw,
                                fh
                            };

                            m_pickups.push_back(p);
                        }

                        if (soundOn && brickSfxCooldown <= 0.0f) {
                            brickBreakSfx.play();
                            brickSfxCooldown = 0.05f; 
                        }

                        if (bricksRemaining <= 0) {
                            currentLevel++;
                            if (currentLevel >= MAX_LEVELS) {
                                // no more levels → victory
                                brickBreakSfx.stop();
                                pickupSfx.stop();
                                gameState = GameState::Victory;
                                if (score > highscore) highscore = score;
                            }
                            else {
                                loadLevel(currentLevel);
                            }
                            break;
                        }
                    }

                    // ALWAYS bounce on hit (not only on destroy)
                    ball.bounceVertical();

                    // Handle only ONE brick per frame
                    break;
                }
            }


        }

        // --- Rendering ---
        SDL_SetRenderDrawColor(app.renderer, 10, 10, 40, 255);
        SDL_RenderClear(app.renderer);

        if (gameState == GameState::Menu)
        {
            if (debug) {SDL_Log("DEBUG: Rendering MENU frame");}

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
			//--- Draw game elements ---
            
            // 1) Bricks
            for (const auto& brick : bricks) {
                brick.render(app.renderer);
            }

			// 2) Falling pickups
            for (const auto& p : m_pickups) {
                p.render(app.renderer);
            }

            // 3) Paddle render
            paddle.render(app.renderer, app.logicalWidth);

			// 4) Ball render
            if (ballTexture) {
                SDL_FRect ballRect = ball.getRect();
                SDL_RenderTexture(app.renderer, ballTexture, nullptr, &ballRect);
            }
        }
        else if (gameState == GameState::Victory)
        {
            ImGui::SetNextWindowPos(
                ImVec2(app.logicalWidth * 0.5f, app.logicalHeight * 0.5f),
                ImGuiCond_Always,
                ImVec2(0.5f, 0.5f)
            );

            ImGui::Begin("Victory!",
                nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav);

            // ---- Centered text ----
            const char* msg = "Victory!";
            float windowWidth = ImGui::GetWindowSize().x;
            float textWidth = ImGui::CalcTextSize(msg).x;

            ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
            ImGui::Text("%s", msg);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // ---- Centered button ----
            const char* btnLabel = "Back to Menu";
            ImVec2 btnSize = ImGui::CalcTextSize(btnLabel);
            btnSize.x += ImGui::GetStyle().FramePadding.x * 2.0f;
            btnSize.y += ImGui::GetStyle().FramePadding.y * 2.0f;

            ImGui::SetCursorPosX((windowWidth - btnSize.x) * 0.5f);
            if (ImGui::Button(btnLabel)) {
                gameState = GameState::Menu;
                brickBreakSfx.stop();
                pickupSfx.stop();
                lives = 3;
                score = 0;
                currentLevel = 0;
                loadLevel(currentLevel);
            }

            ImGui::End();
        }


        // --- React to menu actions (keyboard or ImGui) ---
        if (gameState == GameState::Menu)
        {
            if (quitFromMenu) {
                if (debug) { SDL_Log("DEBUG: quitFromMenu = true -> exiting"); }
                running = false;
            }
            else if (startGame) {
                if (debug) { SDL_Log("DEBUG: startGame = true -> switch to PLAYING"); }
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
    if (upgradeTexture) SDL_DestroyTexture(upgradeTexture);

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    brickBreakSfx.shutdown();
    pickupSfx.shutdown();

    app.shutdown();
    return 0;
    
}   

void Game::changeState(GameStateID newState) {
    currentState = newState;
}
