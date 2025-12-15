
to get sdl

git clone https://github.com/libsdl-org/SDL.git vendored/SDL

to get imgui

git clone https://github.com/libsdl-org/SDL_image.git vendored/SDL_image

# Breakout

High-level modules

SDL / App layer

	SDLApp.h / SDLApp.cpp

Game state & flow
	Game.h / Game.cpp
	GameStates.h / GameStates.cpp
	Menu.h / Menu.cpp

Game objects & logic

	Paddle.h / Paddle.cpp
	Ball.h / Ball.cpp
	Brick.h / Brick.cpp
	BrickLogic.h / BrickLogic.cpp
	Level.h

Audio
	Audio.h / Audio.cpp

UI / Debug UI
	external/imgui/