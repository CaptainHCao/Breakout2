// Breakout.cpp : Defines the entry point for the application.
//

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Breakout.h"
#include <SDL3_image/SDL_image.h>

using namespace std;

struct SDL_State {
	SDL_Window *window;
	SDL_Renderer *renderer;
	int width, height, logicalWidth, logicalHeight;	
};

bool initialize(SDL_State& state);
void cleanup(SDL_State& state);

int main(int argc, char* argv[])
{
	SDL_State state;
	state.width = 1600;
	state.height = 900;
	state.logicalWidth = 640;
	state.logicalHeight = 320;

	if (!initialize(state)) {
		return 1;
	}

	// Asset loading
	SDL_Texture *paddleTexture = IMG_LoadTexture(state.renderer, "assets/paddle.png");
	SDL_SetTextureScaleMode(paddleTexture, SDL_SCALEMODE_NEAREST);
	const float spriteSize = 32;

	// Inputs
	const bool* keys = SDL_GetKeyboardState(nullptr);
	float paddleX = 0;
	float paddleY = 280; // starting Y position
	uint64_t prevTime = SDL_GetTicks();

	// Game loop
	bool running = true;
	while (running) {
		uint64_t nowTime = SDL_GetTicks();
		float deltaTime = (nowTime - prevTime) / 1000.0f;

		SDL_Event event {0 };
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					running = false;
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					state.width = event.window.data1;
					state.height = event.window.data2;
					break;
			}
		}

		// Input handling	
		float paddleSpeed = 150; // per second
		float moveAmount = 0;
		if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
			moveAmount -= paddleSpeed;
		}
		if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
			moveAmount += paddleSpeed;
		}
		paddleX += moveAmount * deltaTime;
		// Wrap paddleX within screen bounds
		if (paddleX < -spriteSize) {
			paddleX = state.logicalWidth - spriteSize;
		}
		else if (paddleX > state.logicalWidth) {
			paddleX = 0;
		}

		// Paddle positioning
		SDL_FRect src{
		.x = 0,
		.y = 0,
		.w = spriteSize,
		.h = spriteSize };

		SDL_FRect dst{
		.x = paddleX,
		.y = paddleY,
		.w = spriteSize,
		.h = spriteSize };
		
		// Background clear
		SDL_SetRenderDrawColor(state.renderer, 40, 40, 40, 255);
		SDL_RenderClear(state.renderer);

		// Render paddle
		SDL_RenderTexture(state.renderer, paddleTexture, &src, &dst);

		// Re-render for wrap-around if needed
		if (paddleX < spriteSize) {
			SDL_FRect wrapDst = dst;
			wrapDst.x = paddleX + state.logicalWidth;
			SDL_RenderTexture(state.renderer, paddleTexture, &src, &wrapDst);
		}
		else if (paddleX + spriteSize > state.logicalWidth - spriteSize) {
			SDL_FRect wrapDst = dst;
			wrapDst.x = paddleX - state.logicalWidth;
			SDL_RenderTexture(state.renderer, paddleTexture, &src, &wrapDst);
		}

		SDL_RenderPresent(state.renderer);

		// Delta time and FPS cap
		prevTime = nowTime;
		SDL_Delay(16); // Roughly 60 FPS
	}

	// Cleanup GPU textures
	SDL_DestroyTexture(paddleTexture);

	cleanup(state);
	return 0;
}

bool initialize(SDL_State& state)
{
	// Initialization code 
	bool success = true;

	// Init SDL3
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to init SDL3", nullptr);
		success = false;
	}

	// Create window
	state.window = SDL_CreateWindow("Breakout Game", state.width, state.height, SDL_WINDOW_RESIZABLE);
	if (!state.window) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create window", nullptr);
		cleanup(state);
		success = false;
		return 1;
	}

	// Create renderer
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if (!state.renderer) {
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
