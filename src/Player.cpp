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
#include "SDL/SDL.h"
#include "SharedData.hpp"
#include "Resources.hpp"
#include "Player.hpp"

Player::Player(Resources *rsc):
    _rsc(rsc),
    _color(Red),
    _dir(Up)
{
    _pos.x = 0;
    _pos.y = 0;
}
        
Player::~Player()
{
}

void
Player::reset()
{
    _color = Red;
    _dir = Up;
    _pos.x = 0;
    _pos.y = 0;
}

void
Player::setOriginAndSize(SDL_Rect rect)
{
    _rect = rect;
}

Player::playerPos
Player::getPos() const
{
    return _pos;
}

Player::playerDirection
Player::getDirection() const
{
    return _dir;
}

void
Player::reverse()
{
    switch (_dir)
    {
        case Up:
            _dir = Down;
            break;
        case Down:
            _dir = Up;
            break;
        case Left:
            _dir = Right;
            break;
        case Right:
            _dir = Left;
            break;
        default:
            break;
    }
}

colors
Player::getColor() const
{
    return _color;
}

void
Player::setColor(colors newColor)
{
    _color = newColor;
}

void
Player::move(playerDirection mov)
{
    if (mov != NoDir)
    {
        // head towards last movement
        _dir = mov;

        //update pos
        switch (mov)
        {
            case Up:
                if (_pos.y > 0)
                    _pos.y--;
                break;
            case Down:
                if (_pos.y < 3)
                    _pos.y++;
                break;
            case Left:
                if (_pos.x > 0)
                    _pos.x--;
                break;
            case Right:
                if (_pos.x < 3)
                    _pos.x++;
                break;
            default:
                break;
        }
    }
}

void
Player::draw()
{
    SDL_Rect src;
    const int blockSize = _rsc->getBlockSize();
    src.x = _color * blockSize;
    src.y = _dir * blockSize;
    src.w = blockSize;
    src.h = blockSize;
    SDL_Rect dst;
    dst.x = _rect.x + _pos.x * blockSize;
    dst.y = _rect.y + _pos.y * blockSize;
    SDL_BlitSurface(_rsc->player(), &src, _rsc->screen(), &dst);
}
