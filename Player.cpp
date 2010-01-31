#include "SDL.h"
#include "SharedData.hpp"
#include "Player.hpp"

namespace
{
    const int bmpSize = 42;
}

Player::Player(SDL_Surface *screen):
    _color(Red),
    _dir(Up),
    _screen(screen)
{
    _pos.x = 0;
    _pos.y = 0;
    _player = SDL_LoadBMP("player.bmp");
}
        
Player::~Player()
{
    SDL_FreeSurface(_player);
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
    src.x = _color * bmpSize;
    src.y = _dir * bmpSize;
    src.w = bmpSize;
    src.h = bmpSize;
    SDL_Rect dst;
    dst.x = _rect.x + _pos.x * bmpSize;
    dst.y = _rect.y + _pos.y * bmpSize;
    dst.w = _rect.w;
    dst.h = _rect.h;
    SDL_BlitSurface(_player, &src, _screen, &dst);
}
