# Add source to this project's executable.

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

add_custom_command(TARGET Breakout POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:Breakout>/assets
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
