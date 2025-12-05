# CMakeList.txt : CMake project for Breakout, include source and define
# project specific logic here.
#

# Add source to this project's executable.
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable (Breakout "Breakout.cpp" "Breakout.h" "GameStates.h" "BallLogic.cpp" "BallLogic.h" "BrickLogic.h" "BrickLogic.cpp")

target_include_directories(Breakout PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_IMAGE_INCLUDE_DIRS})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Breakout PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

target_link_libraries(Breakout PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
