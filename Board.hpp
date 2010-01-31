#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <SDL/SDL.h>
#include "Row.hpp"

class Player;

class Board
{
    public:
        static const int numRows;
        static const int horiRowsLen;
        static const int vertRowsLen;
        static const int rowsInScreen;
        static const int colsInScreen;

        Board(SDL_Surface *screen, Player *player);
        ~Board();

        void resize(int width, int height);
        void draw();

    private:
        SDL_Rect calculateOriginAndTileSize(int width, int height);

        std::vector<Row> _rows;
        SDL_Surface *_board;
        SDL_Surface *_piecesImg;
        SDL_Surface *_screen;
        Player *_player;
};

#endif //BOARD_HPP
