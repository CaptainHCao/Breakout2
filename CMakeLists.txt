cmake_minimum_required(VERSION 3.20)

project("Breakout")

# -------------------------------------------------------------------
# SDL3 + SDL3_image manual configuration (NO find_package)
# -------------------------------------------------------------------
# Adjust these if your SDKs live somewhere else.
set(SDL3_ROOT       "C:/Users/Maria/SDKs/SDL3-3.2.28")
set(SDL3_IMAGE_ROOT "C:/Users/Maria/SDKs/SDL3_image-3.2.4")

# SDL3 imported interface target
add_library(SDL3::SDL3 INTERFACE IMPORTED)
set_target_properties(SDL3::SDL3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL3_ROOT}/include"
    INTERFACE_LINK_LIBRARIES      "${SDL3_ROOT}/lib/x64/SDL3.lib"
)

# SDL3_image imported interface target
add_library(SDL3_image::SDL3_image INTERFACE IMPORTED)
set_target_properties(SDL3_image::SDL3_image PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL3_IMAGE_ROOT}/include"
    INTERFACE_LINK_LIBRARIES      "${SDL3_IMAGE_ROOT}/lib/x64/SDL3_image.lib"
)

# If your libs are actually in lib/x64, then use:
#   "${SDL3_ROOT}/lib/x64/SDL3.lib"
#   "${SDL3_IMAGE_ROOT}/lib/x64/SDL3_image.lib"
# instead.

# -------------------------------------------------------------------
# ImGui library
# -------------------------------------------------------------------
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
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

# Important: link imgui to SDL3 so the backend can see SDL3/SDL.h
target_link_libraries(imgui PUBLIC SDL3::SDL3)

# -------------------------------------------------------------------
# Game sub-project
# -------------------------------------------------------------------
add_subdirectory("Breakout")

add_subdirectory("main.cpp")

add_subdirectory("main")
