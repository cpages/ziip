#include "Board.hpp"

const int numRows = 16;
const int horiRowsLen = 7;
const int vertRowsLen = 5;
const int rowsInScreen = 14;
const int colsInScreen = 18;

Board::Board(SDL_Rect size)
{
    const Row horiRow(horiRowsLen);
    _rows.resize(4, horiRow);
    const Row vertRow(vertRowsLen);
    _rows.resize(8, vertRow);
    _rows.resize(12, horiRow);
    _rows.resize(16, vertRow);
    
    //TODO: calculate proportion and sizes
    //const float proportion = static_cast<float>(rowsInScreen) / colsInScreen;
}
