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
    Row tmpRow(horiRowsLen, 1, 0);
    _rows.resize(4, tmpRow);
    tmpRow = Row(vertRowsLen, 0, -1);
    _rows.resize(8, tmpRow);
    tmpRow = Row(vertRowsLen, -1, 0);
    _rows.resize(12, tmpRow);
    tmpRow = Row(vertRowsLen, 0, 1);
    _rows.resize(16, tmpRow);
    
    //TODO: calculate proportion and sizes
    //const float proportion = static_cast<float>(rowsInScreen) / colsInScreen;

    _board = SDL_LoadBMP("board.bmp");
}

Board::~Board()
{
    SDL_FreeSurface(_board);
}

void
Board::resize(int width, int height)
{
    for (int i = 0; i < numRows; i++)
    {
        // ##### TODO: finish this! #####
        // fill row origins
    }
}

void
Board::draw()
{
    SDL_BlitSurface(_board, NULL, _screen, NULL);
    for (int i = 0; i < numRows; i++)
    {
        _rows[i].draw();
    }
}
