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
#include "SDL.h"
#include "Timer.hpp"
#include "Player.hpp"
#include "Resources.hpp"
#include "Row.hpp"

#define HORI_ROWS 8
#define HORI_ROWS_LEN 7
#define VERT_ROWS 8
#define VERT_ROWS_LEN 5
#define PIECES_IN_BOARD HORI_ROWS * HORI_ROWS_LEN + VERT_ROWS * VERT_ROWS_LEN

class Player;

class Board
{
    public:
        static const int numRows;
        static const int horiRowsLen;
        static const int vertRowsLen;

        struct State
        {
            //std::vector<Color> colsStat;
            Color colsStat[PIECES_IN_BOARD];
            Player::playerPos playerPos;
            Player::playerDirection playerDir;
            Color playerColor;
            int score;
            int sentPieces;
            bool gameOver;
        };

        Board(int id, Resources *rsc);

        void clear();
        bool addPiece();
        void movePlayer(Player::playerDirection mov);
        void playerShooted();
        int getScore() const;
        void gameOver();
        State getState() const;
        void setState(const State &state);
        void draw();

    private:
        class Score
        {
            public:
                Score(int id, Resources *rsc);
                ~Score();

                void reset();
                bool addPoints(int points);
                int getScore() const;
                void setScore(int score);
                void draw();

            private:
                int _id;
                Resources *_rsc;
                int _currScore;
                SDL_Surface *_renderedScore;
        };

        int _id;
        Resources *_rsc;
        Timer _timer;
        Player _player;
        std::vector<Row> _rows;
        int _rowLastPiece;
        Score _score;
        SDL_Rect _rect;
        int _sentPieces; //for net games
        bool _gameOver;
        int _pendZiiped;
};

#endif //BOARD_HPP
