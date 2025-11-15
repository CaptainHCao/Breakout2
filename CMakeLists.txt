cmake_minimum_required(VERSION 3.16)
project(BreakoutGame)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(Breakout
    src/main.cpp
    src/game/Breakout.cpp
    src/ui/Menu.cpp
    src/audio/Audio.cpp
)

add_library(imgui
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_sdlrenderer3.cpp
)


target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)


target_include_directories(Breakout PUBLIC
     ${PROJECT_SOURCE_DIR}/include
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Breakout PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    imgui
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
)
