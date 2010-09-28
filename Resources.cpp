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

    const std::string dataFolder("data/");
    const std::string mainMenuImage("main_menu.png");
    //const std::string mainMenuImage("mm.png");
    const std::string mmNewGameImage("mm_ng.png");
    const std::string mmQuitImage("mm_q.png");
    const std::string mmSelImage("mm_sel.png");
    const std::string piecesImage("pieces.png");
    const std::string boardImage("board.png");
    const std::string playerImage("player.png");
    const std::string gameOverImage("game_over.png");

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
        SDL_Surface *optim = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);
        return optim;
    }
}

Resources::Resources():
    _mMenu(0),
    _mmNG(0),
    _mmQ(0),
    _mmSel(0),
    _pieces(0),
    _board(0),
    _player(0),
    _gOver(0),
    _proportion(1.f),
    _currBlockSize(origPieceSize)
{
    _screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, BITDEPTH,
            (FULLSCREEN? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (_screen == NULL)
    {
        std::ostringstream msg;
        msg << "Could not set video mode: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    _origMMenu = IMG_Load(getFileWithPath(mainMenuImage).c_str());
    _origMMNG = IMG_Load(getFileWithPath(mmNewGameImage).c_str());
    _origMMQ = IMG_Load(getFileWithPath(mmQuitImage).c_str());
    _origMMSel = IMG_Load(getFileWithPath(mmSelImage).c_str());
    _origPieces = IMG_Load(getFileWithPath(piecesImage).c_str());
    _origBoard = IMG_Load(getFileWithPath(boardImage).c_str());
    _origPlayer = IMG_Load(getFileWithPath(playerImage).c_str());
    _origGOver = IMG_Load(getFileWithPath(gameOverImage).c_str());

    prepareBGGraphics();
}

Resources::~Resources()
{
    SDL_FreeSurface(_mMenu);
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
    assert (_mmNG == 0);
    _mmNG = rescaleAndOptimize(_origMMNG, xProp);
    SDL_FreeSurface(_origMMNG);
    assert (_mmQ == 0);
    _mmQ = rescaleAndOptimize(_origMMQ, xProp);
    SDL_FreeSurface(_origMMQ);
    assert (_mmSel == 0);
    _mmSel = rescaleAndOptimize(_origMMSel, xProp);
    SDL_FreeSurface(_origMMSel);
    if (_board != 0)
        SDL_FreeSurface(_board);
    _board = rescaleAndOptimize(_origBoard, xProp);
    SDL_FreeSurface(_origBoard);
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
Resources::mmNG()
{
    return _mmNG;
}

SDL_Surface *
Resources::mmQ()
{
    return _mmQ;
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
