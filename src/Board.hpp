/*
    Copyright 2010 Carles Pagès

    This file is part of Ziip.

    Ziip is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ziip is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ziip.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
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
