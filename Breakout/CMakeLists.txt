# Add source to this project's executable.
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(Breakout
    main.cpp
    Game.cpp
    Menu.cpp
    SDLApp.cpp
    Paddle.cpp
    
    Brick.cpp
    Level.cpp
    Audio.cpp      
    BallLogic.cpp
    GameStates.cpp
    BrickLogic.cpp
)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

# Links against SDL and the imgui target created in the top-level CMakeLists
target_link_libraries(Breakout PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    imgui          
)
