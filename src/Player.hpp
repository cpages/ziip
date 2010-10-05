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
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "SharedData.hpp"

class Player
{
    public:
        enum playerDirection
        {
            Up = 0,
            Down,
            Left,
            Right,
            NoDir
        };

        struct playerPos
        {
            int x;
            int y;
        };

        Player(Resources *rsc);
        ~Player();

        void reset();
        void setOriginAndSize(SDL_Rect rect);
        playerPos getPos() const;
        playerDirection getDirection() const;
        void reverse();
        colors getColor() const;
        void setColor(colors newColor);
        void move(playerDirection mov);
        void draw();

    private:
        Resources *_rsc;
        SDL_Rect _rect;
        playerPos _pos;
        colors _color;
        playerDirection _dir;
};

#endif //PLAYER_HPP
