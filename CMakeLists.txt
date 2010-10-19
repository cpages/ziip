cmake_minimum_required (VERSION 2.6)
project (Ziip)

find_package (SDL REQUIRED)
find_package (SDL_image REQUIRED)
find_package (SDL_ttf REQUIRED)
#cmake doesn't provide a module for SDL_gfx, adding one downloaded from
#http://www.cmake.org/pipermail/cmake/2007-August/015852.html
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package (SDL_gfx REQUIRED)

if (NOT SDL_FOUND OR NOT SDLIMAGE_FOUND OR NOT SDLTTF_FOUND OR
        NOT SDLGFX_FOUND)
    message (FATAL_ERROR "SDL libraries not found")
endif ()

include_directories ("${SDL_INCLUDE_DIR}")
include_directories ("${SDLIMAGE_INCLUDE_DIR}")
include_directories ("${SDLTTF_INCLUDE_DIR}")
include_directories ("${SDLGFX_INCLUDE_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/src")

set (SOURCES
    src/Board.cpp
    src/Config.cpp
    src/HiScore.cpp
    src/InputMgr.cpp
    src/Main.cpp
    src/Player.cpp
    src/Resources.cpp
    src/Row.cpp
    src/Timer.cpp)

add_executable (ziip ${SOURCES})
target_link_libraries (ziip ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY}
    ${SDLTTF_LIBRARY} ${SDLGFX_LIBRARY})
