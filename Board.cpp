#include "Board.hpp"

const int Board::numRows = 16;
const int Board::horiRowsLen = 7;
const int Board::vertRowsLen = 5;
const int Board::rowsInScreen = 14;
const int Board::colsInScreen = 18;

//Board::Board(SDL_Rect size)
Board::Board(SDL_Surface *screen):
    _screen(screen)
{
    const Row horiRow(horiRowsLen);
    _rows.resize(4, horiRow);
    const Row vertRow(vertRowsLen);
    _rows.resize(8, vertRow);
    _rows.resize(12, horiRow);
    _rows.resize(16, vertRow);
    
    //TODO: calculate proportion and sizes
    //const float proportion = static_cast<float>(rowsInScreen) / colsInScreen;

    _board = SDL_LoadBMP("board.bmp");
}

Board::~Board()
{
    SDL_FreeSurface(_board);
}

void
Board::draw()
{
    SDL_BlitSurface(_board, NULL, _screen, NULL);
}
