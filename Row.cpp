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

    colors
    getRandomPiece(colors colorLastPiece)
    {
        int ret;
        if (colorLastPiece == NoColor)
        {
            ret = rand() % 4;
            return static_cast<colors>(ret);
        }
        ret = rand() % (3 + timesSPProbability);
        if (ret < 4)
            return static_cast<colors>(ret);
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
    while (_pieces[i] == NoColor)
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

std::pair<colors, int>
Row::shoot(colors playerColor)
{
    int scoredPoints = 0;
    if (_pieces[0] == NoColor)
        return std::make_pair(NoColor, scoredPoints);
    int i = _size-1;
    while (_pieces[i] == NoColor)
        i--;
    int p = 1; //to give incremental points if more than 1 piece is ziiped
    while (_pieces[i] == playerColor && i >= 0)
    {
        _pieces[i--] = NoColor;
        scoredPoints += basePoints * p++;
    }
    if (i == -1)
        return std::make_pair(playerColor, scoredPoints);
    colors ret = _pieces[i];
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
    while (_pieces[i] != NoColor && i < _size)
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
