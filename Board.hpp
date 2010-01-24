#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <SDL/SDL.h>
#include "Row.hpp"

class Board
{
    public:
        static const int numRows;
        static const int horiRowsLen;
        static const int vertRowsLen;
        static const int rowsInScreen;
        static const int colsInScreen;

        Board(SDL_Rect size);

    private:
        std::vector<Row> _rows;
};

#endif //BOARD_HPP
