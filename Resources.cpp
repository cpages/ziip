#include <string>
#include <cassert>
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "Resources.hpp"

namespace
{
    const int ORIG_WIDTH = 800;
    const int ORIG_HEIGHT = 600;
    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;
    const int origPieceSize = 42;

    const std::string dataFolder("data/");
    const std::string mainMenuImage("main_menu.png");
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
}

Resources::Resources():
    _mMenu(0),
    _pieces(0),
    _board(0),
    _player(0),
    _gOver(0),
    _currPieceSize(origPieceSize)
{

    _screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 0,
            SDL_HWSURFACE | SDL_DOUBLEBUF );

    _origMMenu = IMG_Load(getFileWithPath(mainMenuImage).c_str());
    _origPieces = IMG_Load(getFileWithPath(piecesImage).c_str());
    _origBoard = IMG_Load(getFileWithPath(boardImage).c_str());
    _origPlayer = IMG_Load(getFileWithPath(playerImage).c_str());
    _origGOver = IMG_Load(getFileWithPath(gameOverImage).c_str());

    prepareBGGraphics();
}

Resources::~Resources()
{
    SDL_FreeSurface(_origMMenu);
    SDL_FreeSurface(_mMenu);
    SDL_FreeSurface(_origPieces);
    SDL_FreeSurface(_pieces);
    SDL_FreeSurface(_origBoard);
    SDL_FreeSurface(_board);
    SDL_FreeSurface(_origPlayer);
    SDL_FreeSurface(_player);
    SDL_FreeSurface(_origGOver);
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
    _mMenu = zoomSurface(_origMMenu, xProp, xProp, 0);
    if (_board != 0)
        SDL_FreeSurface(_board);
    _board = zoomSurface(_origBoard, xProp, xProp, 0);
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
Resources::prepareBoardGraphics(float proportion)
{
    _currPieceSize = proportion * origPieceSize;
    _pieces = zoomSurface(_origPieces, proportion, proportion, 0);
    _player = zoomSurface(_origPlayer, proportion, proportion, 0);
    _gOver = zoomSurface(_origGOver, proportion, proportion, 0);
}

void
Resources::getScreenSize(int &width, int &height)
{
    width = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;
}

int
Resources::getPieceSize()
{
    return _currPieceSize;
}
