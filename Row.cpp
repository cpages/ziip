#include <cstdlib>
#include "Row.hpp"

namespace
{
    const int bmpSize = 42;
    //probability to have a piece of the same color as the last in the row (the
    //others being 1)
    const int timesSPProbability = 2;
    const int basePoints = 100;

    colors
    getRandomPiece(colors colorLastPiece)
    {
        int ret = rand() % (3 + timesSPProbability);
        if (ret < 4)
            return static_cast<colors>(ret);
        return colorLastPiece;
    }
}

Row::Row(int size, int growX, int growY, SDL_Surface *screen,
        SDL_Surface *pieces):
    _size(size),
    _pieces(size, NoColor),
    _screen(screen),
    _piecesImg(pieces)
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
    for (++i; i > 0; i--)
    {
        _pieces[i] = _pieces[i-1];
    }
    _pieces[0] = getRandomPiece(_pieces[0]);

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
    while (_pieces[i] != NoColor && i < _size)
    {
        SDL_Rect src;
        src.x = _pieces[i] * bmpSize;
        src.y = 0;
        src.w = bmpSize;
        src.h = bmpSize;
        SDL_Rect dst;
        dst.x = _rect.x + i * _growDir.x * _rect.w;
        dst.y = _rect.y + i * _growDir.y * _rect.h;
        dst.w = _rect.w;
        dst.h = _rect.h;
        SDL_BlitSurface(_piecesImg, &src, _screen, &dst);
        i++;
    }
}
