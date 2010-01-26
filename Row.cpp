#include "Row.hpp"

Row::Row(int size, int growX, int growY):
    _size(size),
    _pieces(size)
{
    _growDir.x = growX;
    _growDir.y = growY;
}

void
Row::setOrigin(int x, int y)
{
}

void
Row::draw()
{
}
