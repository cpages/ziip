cmake_minimum_required (VERSION 2.6)
project (Ziip)
set (Ziip_VERSION_MAJOR 0)
set (Ziip_VERSION_MINOR 1)

#message ("FRP: ${CMAKE_FIND_ROOT_PATH}")

#find_path (HARL SDL.h /include/SDL /usr/include/SDL)
#message ("HARL: ${HARL}")

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package (SDL REQUIRED ttf gfx image)
#find_package (SDL_image REQUIRED)
#find_package (SDL_ttf REQUIRED)
# cmake doesn't provide a module for SDL_gfx, adding one downloaded from
# http://www.cmake.org/pipermail/cmake/2007-August/015852.html
#find_package (SDL_gfx REQUIRED)

if (NOT SDL_FOUND)
    message (FATAL_ERROR "SDL libraries not found")
endif ()

include_directories ("${SDL_INCLUDE_DIR}"
    "${SDLIMAGE_INCLUDE_DIR}"
    "${SDLTTF_INCLUDE_DIR}"
    "${SDLGFX_INCLUDE_DIR}"
    "${PROJECT_SOURCE_DIR}/src")

# set some data for the program to find resources
set (DEST_DATA_DIR share/ziip/data)
set (DEST_FONT_DIR share/ziip/fonts)
set (BUILD_DATA_FOLDER ${PROJECT_SOURCE_DIR}/data/)
set (INSTALL_DATA_FOLDER ${CMAKE_INSTALL_PREFIX}/${DEST_DATA_DIR}/)
set (BUILD_FONT_FOLDER ${PROJECT_SOURCE_DIR}/fonts/)
set (INSTALL_FONT_FOLDER ${CMAKE_INSTALL_PREFIX}/${DEST_FONT_DIR}/)
configure_file ("${PROJECT_SOURCE_DIR}/src/SharedData.hpp.in"
    "${PROJECT_BINARY_DIR}/src/SharedData.hpp")
include_directories ("${PROJECT_BINARY_DIR}/src")

# with this option we define where ziip will look for the data and fonts
option (USE_LOCAL "Whether you plan to run Ziip without installing" OFF)

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

install (TARGETS ziip DESTINATION bin)
install (DIRECTORY data/ DESTINATION ${DEST_DATA_DIR})
install (DIRECTORY fonts/ DESTINATION ${DEST_FONT_DIR})
