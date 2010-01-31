#include <cstdlib>
#include "Row.hpp"

namespace
{
    const int bmpSize = 42;
    //probability to have a piece of the same color as the last in the row (the
    //others being 1)
    const int timesSPProbability = 2;

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

colors
Row::shoot(colors playerColor)
{
    if (_pieces[0] == NoColor)
        return NoColor;
    int i = _size-1;
    while (_pieces[i] == NoColor)
        i--;
    while (_pieces[i] == playerColor && i >= 0)
    {
        _pieces[i] = NoColor;
        i--;
    }
    if (i == -1)
        return playerColor;
    colors ret = _pieces[i];
    _pieces[i] = playerColor;
    return ret;
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
