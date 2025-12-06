# Add source to this project's executable.
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(Breakout
    "Main.cpp"
    "Menu.cpp"
    "SDLApp.h"
    "SDLApp.cpp"
    "Paddle.h"
    "Paddle.cpp"
    "Game.h"
    "Game.cpp"
    "Ball.h"
    "Brick.h"
    "Level.h"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

# Link against SDL and the imgui target created in the top-level CMakeLists
target_link_libraries(Breakout PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    imgui           # <--- this is the target you created above
)
