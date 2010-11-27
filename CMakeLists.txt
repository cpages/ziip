cmake_minimum_required (VERSION 2.6)
project (Ziip)
set (Ziip_VERSION_MAJOR 0)
set (Ziip_VERSION_MINOR 1)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package (SDL REQUIRED ttf gfx image net)

include_directories ("${SDL_INCLUDE_DIRS}"
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

set (ADDITIONAL_DEPS "")
if (CMAKE_CROSSCOMPILING)
    option (BUILD_WII "Cross compile for the Wii" ON)
    if (BUILD_WII)
        #TODO: the flags are not marked in the cache
        set (CMAKE_C_FLAGS ${WII_MACHDEP})
        set (CMAKE_CXX_FLAGS ${WII_MACHDEP})
        set (ADDITIONAL_DEPS ${ADDITIONAL_DEPS} ${WII_DEPS})
    endif ()
endif ()

# with this option we define where ziip will look for the data and fonts
option (USE_LOCAL "Whether you plan to run Ziip without installing" OFF)

set (SOURCES
    src/Board.cpp
    src/Config.cpp
    src/HiScore.cpp
    src/InputMgr.cpp
    src/Main.cpp
    src/Menu.cpp
    src/Net.cpp
    src/Player.cpp
    src/Resources.cpp
    src/Row.cpp
    src/SharedData.cpp
    src/Timer.cpp)

add_executable (ziip ${SOURCES})
target_link_libraries (ziip ${SDL_LIBRARIES} ${ADDITIONAL_DEPS})

install (TARGETS ziip DESTINATION bin)
install (DIRECTORY data/ DESTINATION ${DEST_DATA_DIR})
install (DIRECTORY fonts/ DESTINATION ${DEST_FONT_DIR})

if (CMAKE_CROSSCOMPILING)
    if (BUILD_WII)
        add_custom_command (TARGET ziip
            POST_BUILD
            COMMAND /opt/devkitpro/devkitPPC/bin/elf2dol ziip ziip.dol)
    endif ()
endif ()
