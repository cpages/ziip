#include "Row.hpp"

namespace
{
    const int bmpSize = 42;

    colors
    getRandomPiece(colors color)
    {
        static int col = 0;
        col++;
        if (col > 3)
            col = 0;
        switch (col)
        {
            case 0:
                return Red;
            case 1:
                return Green;
            case 2:
                return Blue;
            case 3:
                return Purple;
        }
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
