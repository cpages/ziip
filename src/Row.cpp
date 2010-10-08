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
#include <iostream>
#include <cstdlib>
#include "Resources.hpp"
#include "Row.hpp"

namespace
{
    const bool debug = false;
    //probability to have a piece of the same color as the last in the row (the
    //others being 1)
    const int timesSPProbability = 2;
    const int basePoints = 100;

    Color
    getRandomPiece(Color colorLastPiece)
    {
        int ret;
        if (colorLastPiece == NoColor)
        {
            ret = rand() % 4;
            return static_cast<Color>(ret);
        }
        ret = rand() % (3 + timesSPProbability);
        if (ret < 4)
            return static_cast<Color>(ret);
        return colorLastPiece;
    }
}

Row::Row(int size, int growX, int growY, Resources *rsc):
    _rsc(rsc),
    _size(size),
    _pieces(size, NoColor)
{
    _growDir.x = growX;
    _growDir.y = growY;
}

void
Row::clear()
{
    _pieces.assign(_size, NoColor);
}

bool
Row::addPiece()
{
    if (_pieces[_size-1] != NoColor)
        return true;
    int i = _size-1;
    while (i >= 0 && _pieces[i] == NoColor)
        i--;
    if (debug)
        std::cout << i+1 << " pieces in row" << std::endl;
    for (++i; i > 0; i--)
    {
        _pieces[i] = _pieces[i-1];
    }
    _pieces[0] = getRandomPiece(_pieces[0]);
    if (debug)
        std::cout << "Added " << _pieces[0] << std::endl;

    return false;
}

std::pair<Color, int>
Row::shoot(Color playerColor)
{
    int scoredPoints = 0;
    if (_pieces[0] == NoColor)
        return std::make_pair(NoColor, scoredPoints);
    int i = _size-1;
    while (_pieces[i] == NoColor)
        i--;
    int p = 1; //to give incremental points if more than 1 piece is ziiped
    while (i >= 0 && _pieces[i] == playerColor)
    {
        _pieces[i--] = NoColor;
        scoredPoints += basePoints * p++;
    }
    if (i == -1)
        return std::make_pair(playerColor, scoredPoints);
    Color ret = _pieces[i];
    _pieces[i] = playerColor;
    return std::make_pair(ret, scoredPoints);
}

void
Row::setOriginAndSize(SDL_Rect rect)
{
    _rect = rect;
}

void
Row::draw()
{
    int i = 0;
    const int pieceSize = _rsc->getBlockSize();
    while (i < _size && _pieces[i] != NoColor)
    {
        SDL_Rect src;
        src.x = _pieces[i] * pieceSize;
        src.y = 0;
        src.w = pieceSize;
        src.h = pieceSize;
        SDL_Rect dst;
        dst.x = _rect.x + i * _growDir.x * _rect.w;
        dst.y = _rect.y + i * _growDir.y * _rect.h;
        SDL_BlitSurface(_rsc->pieces(), &src, _rsc->screen(), &dst);
        i++;
    }
}
