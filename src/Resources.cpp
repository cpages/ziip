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
        "board.png",
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
        SDL_Surface *tmp = zoomSurface(orig, proportion, proportion, 0);
        SDL_Surface *optim = SDL_DisplayFormatAlpha(tmp);
        SDL_FreeSurface(tmp);
        return optim;
    }
}

Resources::Resources():
    _mMenu(0),
    _mmSel(0),
    _pieces(0),
    _board(0),
    _player(0),
    _gOver(0),
    _proportion(1.f),
    _currBlockSize(origPieceSize)
{
    for (int i = 0; i < NumSurfaces; ++i)
        _surfaceFiles.push_back(std::string(surfaceFiles[i]));

    _screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, BITDEPTH,
            (FULLSCREEN? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (_screen == NULL)
    {
        std::ostringstream msg;
        msg << "Could not set video mode: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    _origMMenu = IMG_Load(getFileWithPath(_surfaceFiles[SfcMainMenu]).c_str());
    _origMMSel = IMG_Load(getFileWithPath(_surfaceFiles[SfcMMSel]).c_str());
    _origPieces = IMG_Load(getFileWithPath(_surfaceFiles[SfcPieces]).c_str());
    _origBoard = IMG_Load(getFileWithPath(_surfaceFiles[SfcBoard]).c_str());
    _origPlayer = IMG_Load(getFileWithPath(_surfaceFiles[SfcPlayer]).c_str());
    _origGOver = IMG_Load(getFileWithPath(_surfaceFiles[SfcGameOver]).c_str());

    prepareBGGraphics();
}

Resources::~Resources()
{
    SDL_FreeSurface(_mMenu);
    SDL_FreeSurface(_mmSel);
    SDL_FreeSurface(_pieces);
    SDL_FreeSurface(_board);
    SDL_FreeSurface(_player);
    SDL_FreeSurface(_gOver);
}

void
Resources::prepareBGGraphics()
{
    const float xProp = static_cast<float>(WINDOW_WIDTH)/ORIG_WIDTH;
    const float yProp = static_cast<float>(WINDOW_HEIGHT)/ORIG_HEIGHT;
    assert (xProp == yProp);

    if (_mMenu != 0)
        SDL_FreeSurface(_mMenu);
    _mMenu = rescaleAndOptimize(_origMMenu, xProp);
    SDL_FreeSurface(_origMMenu);
    assert (_mmSel == 0);
    _mmSel = rescaleAndOptimize(_origMMSel, xProp);
    SDL_FreeSurface(_origMMSel);
    if (_board != 0)
        SDL_FreeSurface(_board);
    SDL_Surface *har = IMG_Load(getFileWithPath(std::string("bg01.png")).c_str());
    _board = rescaleAndOptimize(har, xProp);
    SDL_Surface *hor = rescaleAndOptimize(_origBoard, xProp);
    SDL_BlitSurface(hor, NULL, _board, NULL);
    SDL_FreeSurface(_origBoard);
    SDL_FreeSurface(har);
    SDL_FreeSurface(hor);
}

SDL_Surface *
Resources::screen()
{
    return _screen;
}

SDL_Surface *
Resources::mainMenu()
{
    return _mMenu;
}

SDL_Surface *
Resources::mmSel()
{
    return _mmSel;
}

SDL_Surface *
Resources::pieces()
{
    return _pieces;
}

SDL_Surface *
Resources::board()
{
    return _board;
}

SDL_Surface *
Resources::player()
{
    return _player;
}

SDL_Surface *
Resources::gameOver()
{
    return _gOver;
}

void
Resources::prepareBoardGraphics(int newBlockSize)
{
    _currBlockSize = newBlockSize;
    const float _proportion = static_cast<float>(newBlockSize) / origPieceSize;
    if (_pieces != 0)
        SDL_FreeSurface(_pieces);
    _pieces = rescaleAndOptimize(_origPieces, _proportion);
    SDL_FreeSurface(_origPieces);
    if (_player != 0)
        SDL_FreeSurface(_player);
    _player = rescaleAndOptimize(_origPlayer, _proportion);
    SDL_FreeSurface(_origPlayer);
    if (_gOver != 0)
        SDL_FreeSurface(_gOver);
    _gOver = rescaleAndOptimize(_origGOver, _proportion);
    SDL_FreeSurface(_origGOver);
}

void
Resources::getScreenSize(int &width, int &height)
{
    width = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;
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
