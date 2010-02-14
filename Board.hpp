#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include "SDL.h"
#include "SDL_ttf.h"
#include "Timer.hpp"
#include "Player.hpp"
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

        Board(Resources *rsc);
        ~Board();

        void clear();
        bool addPiece();
        void movePlayer(Player::playerDirection mov);
        void playerShooted();
        void resize(int width, int height);
        void draw();

    private:
        class Score
        {
            public:
                Score(Resources *rsc);
                ~Score();

                void reset();
                bool addPoints(int points);
                void draw();

            private:
                Resources *_rsc;
                int _currScore;
                TTF_Font *_font;
                SDL_Surface *_renderedScore;
        };

        SDL_Rect calculateOriginAndTileSize(int width, int height);

        Resources *_rsc;
        Timer _timer;
        Player _player;
        std::vector<Row> _rows;
        int _rowLastPiece;
        SDL_Surface *_board;
        SDL_Surface *_piecesImg;
        Score _score;
        SDL_Rect _origSize;
};

#endif //BOARD_HPP
