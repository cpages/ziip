/*
    Copyright 2010 Carles Pagès

    This file is part of Ziip.

    Ziip is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ziip is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ziip.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cassert>
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include "Resources.hpp"

namespace
{
    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;
    const int BITDEPTH = 0; //same as current display
    const bool FULLSCREEN = false;
    const int ORIG_WIDTH = 800;
    const int ORIG_HEIGHT = 600;
    const int origPieceSize = 42;
    const int rowsInGrid = 14;
    const int colsInGrid = 18;

#ifdef GEKKO
    const std::string dataFolder("sd:/apps/ziip/data/");
#else
    const std::string dataFolder("data/");
#endif
    const char *surfaceFiles[] = {
        "",
        "main_menu.png",
        "mm_sel.png",
        "pieces.png",
        "",
        "grid.png",
        "player.png",
        "game_over.png"
    };

    std::string
    getFileWithPath(std::string filename)
    {
        std::string ret = dataFolder;
        return ret.append(filename);
    }

    SDL_Surface *
    rescaleAndOptimize(SDL_Surface *orig, float proportion)
    {
        //TODO: error checking
        SDL_Surface *tmp = zoomSurface(orig, proportion, proportion, 0);
        SDL_Surface *optim = SDL_DisplayFormatAlpha(tmp);
        SDL_FreeSurface(tmp);
        return optim;
    }
}

Resources::Resources():
    _winWidth(WINDOW_WIDTH),
    _winHeight(WINDOW_HEIGHT),
    _proportion(1.f),
    _currBlockSize(origPieceSize)
{
    for (int i = 0; i < NumSurfaces; ++i)
        _surfaceFiles.push_back(std::string(surfaceFiles[i]));
    _surfaces.resize(NumSurfaces, 0);

    //TODO: load all available backgrounds
    _bgFiles.push_back(std::string("bg00.png"));

    //this surface is freed by SDL_Quit
    SDL_Surface *screen = SDL_SetVideoMode( _winWidth, _winHeight,
            BITDEPTH, (FULLSCREEN? SDL_FULLSCREEN : 0) | SDL_HWSURFACE |
            SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        std::ostringstream msg;
        msg << "Could not set video mode: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
    _surfaces[SfcScreen] = screen;

    prepareBGGraphics();
}

Resources::~Resources()
{
    //we don't want to free the screen, SDL_Quit does this
    for (int i = 1; i < NumSurfaces; ++i)
        SDL_FreeSurface(_surfaces[i]);
}

void
Resources::prepareSurface(Surface sfc, float prop)
{
    if (_surfaces[sfc])
        SDL_FreeSurface(_surfaces[sfc]);
    SDL_Surface *origSfc =
        IMG_Load(getFileWithPath(_surfaceFiles[sfc]).c_str());
    if (!origSfc)
    {
        std::ostringstream msg;
        msg << "Could not load image: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
    _surfaces[sfc] = rescaleAndOptimize(origSfc, prop);
    SDL_FreeSurface(origSfc);
}

void
Resources::prepareBGGraphics()
{
    const float xProp = static_cast<float>(_winWidth)/ORIG_WIDTH;
    const float yProp = static_cast<float>(_winHeight)/ORIG_HEIGHT;
    assert (xProp == yProp);

    prepareSurface(SfcMainMenu, xProp);
    prepareSurface(SfcMMSel, xProp);
}

void
Resources::prepareBoardGraphics(int numPlayers)
{
    //TODO: multiple screens
    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = _winWidth;
    rect.h = _winHeight;
    _boardAreas.clear();
    _boardAreas.push_back(rect);

    const float gridProp = static_cast<float>(rowsInGrid) / colsInGrid;
    //find origin and size of grid
    SDL_Rect ret;
    if (static_cast<float>(rect.h) / rect.w > gridProp)
        ret.w = ret.h = rect.w / colsInGrid;
    else
        ret.w = ret.h = rect.h / rowsInGrid;
    ret.x = (rect.w - ret.w * colsInGrid) / 2;
    ret.y = (rect.h - ret.h * rowsInGrid) / 2;
    _gridAreas.clear();
    _gridAreas.push_back(ret);

    const float xProp = static_cast<float>(_winWidth)/ORIG_WIDTH;
    const float yProp = static_cast<float>(_winHeight)/ORIG_HEIGHT;
    assert (xProp == yProp);

    _surfaceFiles[SfcBoard] = _bgFiles[0];
    prepareSurface(SfcBoard, xProp);

    const int newBlockSize = 33;
    _currBlockSize = newBlockSize;
    const float _proportion = static_cast<float>(newBlockSize) / origPieceSize;

    prepareSurface(SfcGrid, xProp);
    //we blit the grid to the background to get the final board
    SDL_BlitSurface(_surfaces[SfcGrid], NULL, _surfaces[SfcBoard], NULL);

    prepareSurface(SfcPieces, _proportion);
    prepareSurface(SfcPlayer, _proportion);
    prepareSurface(SfcGameOver, _proportion);
}

SDL_Surface *
Resources::screen()
{
    return _surfaces[SfcScreen];
}

SDL_Surface *
Resources::getSfc(Surface sfc)
{
    return _surfaces[sfc];
}

void
Resources::getScreenSize(int &width, int &height)
{
    width = _winWidth;
    height = _winHeight;
}

float
Resources::getProportion()
{
    return _proportion;
}

int
Resources::getBlockSize()
{
    return _currBlockSize;
}

SDL_Rect
Resources::getBoardArea(int id) const
{
    return _boardAreas[id];
}

SDL_Rect
Resources::getGridArea(int id) const
{
    return _gridAreas[id];
}
