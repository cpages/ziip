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

        Board(SDL_Surface *screen);
        ~Board();

        void resize(int width, int height);
        void draw();

    private:
        std::vector<Row> _rows;
        SDL_Surface *_board;
        SDL_Surface *_screen;
};

#endif //BOARD_HPP
