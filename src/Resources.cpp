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
#include <cmath>
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "SharedData.hpp"
#include "Resources.hpp"

namespace
{
    const int BITDEPTH = 0; //same as current display
#ifdef GEKKO
    const bool FULLSCREEN = false;
#else
    const bool FULLSCREEN = false;
#endif
    const int ORIG_WIDTH = 800;
    const int ORIG_HEIGHT = 600;
    const int origBlockSize = 42;
    const int rowsInGrid = 14;
    const int colsInGrid = 18;

#ifdef GEKKO
    const std::string dataFolder("sd:/apps/ziip/data/");
    const std::string fontFolder("sd:/apps/ziip/fonts/");
#else
#ifdef USE_LOCAL
    const std::string dataFolder(BUILD_DATA_FOLDER);
    const std::string fontFolder(BUILD_FONT_FOLDER);
#else
    const std::string dataFolder(INSTALL_DATA_FOLDER);
    const std::string fontFolder(INSTALL_FONT_FOLDER);
#endif
#endif
    const std::string fontName("LiberationMono-Bold.ttf");
    const int fontSize = 24;
    const char *surfaceFiles[] = {
        "",
        "main_menu.png",
        "mp_menu.png",
        "mm_sel.png",
        "pieces.png",
        "",
        "grid.png",
        "player.png",
        "bg00.png"
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
        SDL_Surface *tmp = zoomSurface(orig, proportion, proportion, 1);
        SDL_Surface *optim = SDL_DisplayFormatAlpha(tmp);
        SDL_FreeSurface(tmp);
        return optim;
    }
}

Resources::Resources():
    _winWidth(_cfg.getWinWidth()),
    _winHeight(_cfg.getWinHeight()),
    _inputMgr(_cfg.getControls()),
    _blockScale(1.f),
    _currBlockSize(origBlockSize)
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

Resources::FontMgr::FontMgr()
{
    //init TTF stuff
    if (TTF_Init() == -1)
    {
        std::ostringstream msg;
        msg << "Could not initialize SDL_ttf: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    std::string fontFile = fontFolder;
    fontFile.append(fontName);
    _font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (_font == NULL)
    {
        std::ostringstream msg;
        msg << "Could not load font: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
}

Resources::FontMgr::~FontMgr()
{
    TTF_CloseFont(_font); 
    TTF_Quit();
}

SDL_Surface *
Resources::FontMgr::renderText(const std::string &str, const SDL_Color &color)
{
    return TTF_RenderText_Solid(_font, str.c_str(), color);
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
    prepareSurface(SfcMPMenu, xProp);
    prepareSurface(SfcMMSel, xProp);
    prepareSurface(SfcHiScores, xProp);
}

void
Resources::divideScreen(int numPlayers)
{
    _boardAreas.clear();
    const float sq = std::sqrt(float(numPlayers));
    const int colDiv = std::ceil(sq);
    const int rowDiv = std::ceil(float(numPlayers)/colDiv);
    const int boardW = std::floor(float(_winWidth)/colDiv);
    const int boardH = std::floor(float(_winHeight)/rowDiv);
    for (int i = 0; i < rowDiv; ++i)
    {
        for (int j = 0; j < colDiv; ++j)
        {
            //there might be some empty parts
            if (i * colDiv + j == numPlayers)
                return;

            SDL_Rect rect;
            rect.x = j * boardW;
            rect.y = i * boardH;
            rect.w = boardW;
            rect.h = boardH;
            _boardAreas.push_back(rect);
        }
    }
}

void
Resources::fillGridRects()
{
    //find origin and size of grid
    const float xProp = float(_boardAreas[0].w) / (colsInGrid * origBlockSize);
    const float yProp = float(_boardAreas[0].h) / (rowsInGrid * origBlockSize);
    const float proportion = std::min(xProp, yProp);
    _currBlockSize = std::floor(origBlockSize * proportion);
    _blockScale = float(_currBlockSize) / origBlockSize;
    const int gridW = colsInGrid * _currBlockSize;
    const int gridH = rowsInGrid * _currBlockSize;
    const int xOffset = std::floor(float(_boardAreas[0].w - gridW) / 2);
    const int yOffset = std::floor(float(_boardAreas[0].h - gridH) / 2);

    _gridAreas.clear();
    for (unsigned int i = 0; i < _boardAreas.size(); ++i)
    {
        SDL_Rect gridRect = _boardAreas[i];
        gridRect.x += xOffset;
        gridRect.y += yOffset;
        //TODO: is this necessary?
        gridRect.w = gridW;
        gridRect.h = gridH;
        _gridAreas.push_back(gridRect);
    }
}

void
Resources::prepareBoardGraphics(int numPlayers)
{
    divideScreen(numPlayers);
    fillGridRects();

    const float xProp = static_cast<float>(_winWidth)/ORIG_WIDTH;
    const float yProp = static_cast<float>(_winHeight)/ORIG_HEIGHT;
    assert (xProp == yProp);
    _surfaceFiles[SfcBoard] = _bgFiles[0];
    prepareSurface(SfcBoard, xProp);

    prepareSurface(SfcGrid, _blockScale);
    for (unsigned int i = 0; i < _gridAreas.size(); ++i)
    {
        //we blit the grid to the background to get the final board
        SDL_BlitSurface(_surfaces[SfcGrid], NULL,
                _surfaces[SfcBoard], &_gridAreas[i]);
    }

    prepareSurface(SfcPieces, _blockScale);
    prepareSurface(SfcPlayer, _blockScale);
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
Resources::getBgScale()
{
    return float(_winWidth) / ORIG_WIDTH;
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

SDL_Surface *
Resources::renderText(const std::string &str, const SDL_Color &color)
{
    return _fontMgr.renderText(str, color);
}

InputMgr::KeyPressed
Resources::getKey(const SDL_Event &event)
{
    return _inputMgr(event);
}
