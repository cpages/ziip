#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include "SDL.h"
#include "SDL_ttf.h"
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

        bool addPiece();
        void playerShooted();
        void resize(int width, int height);
        void draw();

    private:
        class Score
        {
            public:
                Score(SDL_Surface *screen);
                ~Score();

                void addPoints(int points);
                void draw();

            private:
                int _currScore;
                SDL_Surface *_screen;
                TTF_Font *_font;
                SDL_Surface *_renderedScore;
        };

        SDL_Rect calculateOriginAndTileSize(int width, int height);

        std::vector<Row> _rows;
        int _rowLastPiece;
        SDL_Surface *_board;
        SDL_Surface *_piecesImg;
        SDL_Surface *_screen;
        Player *_player;
        Score _score;
};

#endif //BOARD_HPP
