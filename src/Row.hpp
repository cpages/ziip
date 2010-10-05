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
#ifndef ROW_HPP
#define ROW_HPP

#include <vector>
#include <utility>
#include "SharedData.hpp"

class Row
{
    public:
        Row(int size, int growX, int growY, Resources *rsc);

        void clear();
        bool addPiece();
        std::pair<colors, int> shoot(colors playerColor);
        void setOriginAndSize(SDL_Rect rect);
        void draw();

    private:
        Resources *_rsc;
        int _size;
        SDL_Rect _growDir;
        SDL_Rect _rect;
        std::vector<colors> _pieces;
};

#endif //ROW_HPP
