#include "Row.hpp"

namespace
{
    const int bmpSize = 42;
}

Row::Row(int size, int growX, int growY, SDL_Surface *screen,
        SDL_Surface *pieces):
    _size(size),
    _pieces(size),
    _screen(screen),
    _piecesImg(pieces)
{
    _growDir.x = growX;
    _growDir.y = growY;

    //tmp
    for (int i = 0; i < _size; i++)
        _pieces[i] = Red;
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
