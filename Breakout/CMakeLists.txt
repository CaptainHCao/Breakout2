# CMakeList.txt : CMake project for Breakout, include source and define
# project specific logic here.

# Add source to this project's executable.
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

<<<<<<< Updated upstream
add_executable (Breakout "Breakout.cpp" "Breakout.h" "GameStates.h" "BallLogic.cpp" "BallLogic.h" "BrickLogic.h" "BrickLogic.cpp")

target_include_directories(Breakout PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_IMAGE_INCLUDE_DIRS})

=======
add_executable (Breakout "Breakout.cpp" "Menu.cpp" "SDLApp.h" "SLDApp.cpp" "Paddle.h" "Paddle.cpp" "Game.h" "Game.cpp" "Ball.h" "Brick.h" "Level.h")
>>>>>>> Stashed changes

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

<<<<<<< Updated upstream
target_link_libraries(Breakout PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

target_link_libraries(Breakout PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
=======
target_link_libraries(Breakout PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    imgui           
)
>>>>>>> Stashed changes
