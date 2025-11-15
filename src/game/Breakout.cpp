// Breakout.cpp : Defines the entry point for the application.
//

#include "Breakout.h"
#include "Menu.h"
#include <SDL3_image/SDL_image.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

struct SDL_State
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	int width, height, logicalWidth, logicalHeight;
};

bool initialize(SDL_State &state);
void cleanup(SDL_State &state);

void Breakout::run()
{
	SDL_State state;
	state.width = 1600;
	state.height = 900;
	state.logicalWidth = 640;
	state.logicalHeight = 320;

	if (!initialize(state))
	{
		return;
	}

	// ImGui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForSDLRenderer(state.window, state.renderer);
	ImGui_ImplSDLRenderer3_Init(state.renderer);

	// --- Game / menu state ---
	enum class GameState
	{
		Menu,
		Playing
	};

	GameState gameState = GameState::Menu;

	bool soundOn = true;
	bool musicOn = true;
	int highscore = 0;

	Menu menu; // defined in Menu.h / Menu.cpp

	// Asset loading for gameplay
	SDL_Texture *paddleTexture = IMG_LoadTexture(state.renderer, "assets/textures/paddle.png");

	if (!paddleTexture)
	{
		SDL_Log("Failed to load texture: %s", SDL_GetError());
		return;
	}

	SDL_Texture *bgTexture = IMG_LoadTexture(state.renderer, "assets/textures/dragon.png");

	SDL_SetTextureScaleMode(bgTexture, SDL_SCALEMODE_NEAREST);

	SDL_SetTextureScaleMode(paddleTexture, SDL_SCALEMODE_NEAREST);
	const float spriteSize = 32.0f;

	// Inputs
	const bool *keys = SDL_GetKeyboardState(nullptr);
	float paddleX = 0.0f;
	float paddleY = 280.0f; // starting Y position
	uint64_t prevTime = SDL_GetTicks();

	bool running = true;

	// Game loop
	while (running)
	{
		// --- Time step ---
		const uint64_t nowTime = SDL_GetTicks();
		const float deltaTime = (nowTime - prevTime) / 1000.0f;
		prevTime = nowTime;

		// Per–frame menu actions (set by keyboard and ImGui)
		bool startGame = false;
		bool quitFromMenu = false;

		// --- Event handling ---
		SDL_Event event{0};
		while (SDL_PollEvent(&event))
		{
			// Let ImGui consume all events first
			ImGui_ImplSDL3_ProcessEvent(&event);

			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				running = false;
				break;

			case SDL_EVENT_WINDOW_RESIZED:
				state.width = event.window.data1;
				state.height = event.window.data2;
				break;

			case SDL_EVENT_KEY_DOWN:
				if (gameState == GameState::Menu)
				{
					// Keyboard controls for the menu (ENTER, ESC, etc.)
					menu.handleEvent(event, soundOn, musicOn, highscore,
									 startGame, quitFromMenu);
				}
				else if (gameState == GameState::Playing)
				{
					// Allow ESC to go back to the menu while playing
					if (event.key.key == SDLK_ESCAPE)
					{
						gameState = GameState::Menu;
					}
				}
				break;

			default:
				break;
			}
		}

		// --- Start ImGui frame ---
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		{
			float scaleX, scaleY;
			SDL_GetRenderScale(state.renderer, &scaleX, &scaleY);
			ImGuiIO &io = ImGui::GetIO();
			io.DisplayFramebufferScale = ImVec2(scaleX, scaleY);
		}

		// --- Game update (only when playing) ---
		if (gameState == GameState::Playing)
		{
			const float paddleSpeed = 150.0f; // pixels per second
			float moveAmount = 0.0f;

			if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
			{
				moveAmount -= paddleSpeed;
			}
			if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
			{
				moveAmount += paddleSpeed;
			}

			paddleX += moveAmount * deltaTime;

			// Wrap paddleX within logical screen bounds
			if (paddleX < -spriteSize)
			{
				paddleX = state.logicalWidth - spriteSize;
			}
			else if (paddleX > state.logicalWidth)
			{
				paddleX = 0.0f;
			}
		}

		// --- Rendering ---
		SDL_SetRenderDrawColor(state.renderer, 10, 10, 40, 255);
		SDL_RenderClear(state.renderer);

		if (gameState == GameState::Menu)
		{
			// ImGui–driven start screen
			SDL_FRect bgRect{
				.x = 0.0f,
				.y = 0.0f,
				.w = (float)state.logicalWidth,
				.h = (float)state.logicalHeight};
			SDL_RenderTexture(state.renderer, bgTexture, nullptr, &bgRect);

			// 2) draw UI on top
			menu.render(state.renderer,
						soundOn, musicOn, highscore,
						startGame, quitFromMenu,
						state.logicalWidth, state.logicalHeight);
		}
		else if (gameState == GameState::Playing)
		{
			// Paddle sprite
			SDL_FRect src{
				.x = 0.0f,
				.y = 0.0f,
				.w = spriteSize,
				.h = spriteSize};

			SDL_FRect dst{
				.x = paddleX,
				.y = paddleY,
				.w = spriteSize,
				.h = spriteSize};

			SDL_RenderTexture(state.renderer, paddleTexture, &src, &dst);

			// Wrap–around rendering if needed
			if (paddleX < spriteSize)
			{
				SDL_FRect wrapDst = dst;
				wrapDst.x = paddleX + state.logicalWidth;
				SDL_RenderTexture(state.renderer, paddleTexture, &src, &wrapDst);
			}
			else if (paddleX + spriteSize > state.logicalWidth - spriteSize)
			{
				SDL_FRect wrapDst = dst;
				wrapDst.x = paddleX - state.logicalWidth;
				SDL_RenderTexture(state.renderer, paddleTexture, &src, &wrapDst);
			}
		}

		// --- React to menu actions (from keyboard or buttons) ---
		if (quitFromMenu)
		{
			running = false;
		}
		else if (startGame && gameState == GameState::Menu)
		{
			gameState = GameState::Playing;

			// Reset gameplay state when entering the game
			paddleX = 0.0f;
			paddleY = 280.0f;
		}

		// --- ImGui + present ---
		ImGui::Render();
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state.renderer);

		SDL_RenderPresent(state.renderer);

		// Simple frame cap (~60 FPS)
		SDL_Delay(16);
	}

	// Cleanup GPU textures
	SDL_DestroyTexture(paddleTexture);
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	cleanup(state);
}

bool initialize(SDL_State &state)
{
	// Initialization code
	bool success = true;

	// Init SDL3
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to init SDL3", nullptr);
		success = false;
	}

	// SDL_CreateWindowAndRenderer("Breakout Game", 800, 600, SDL_WINDOW_RESIZABLE, &state.window, &state.renderer);
	// if (!state->window || !state->renderer){
	// 	SDL_Log("Failed to create window or renderer: %s", SDL_GetError());
	// 	SDL_DestroyWindow(state->window);
	// 	return SDL_APP_FAILURE;
	// }

	// Create window
	state.window = SDL_CreateWindow("Breakout Game", state.width, state.height, SDL_WINDOW_RESIZABLE);
	if (!state.window)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create window", nullptr);
		cleanup(state);
		success = false;
		return 1;
	}

	// Create renderer
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if (!state.renderer)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create renderer", state.window);
		cleanup(state);
		success = false;
		return 1;
	}

	// Config presentation
	SDL_SetRenderLogicalPresentation(state.renderer, state.logicalWidth, state.logicalHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	return success;
}

void cleanup(SDL_State &state)
{
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
}
