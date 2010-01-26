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
Player::setOrigin(int x, int y)
{
    _origin.x = x;
    _origin.y = y;
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
                _pos.y--;
                break;
            case Down:
                _pos.y++;
                break;
            case Left:
                _pos.x--;
                break;
            case Right:
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
    dst.x = _origin.x + _pos.x * bmpSize;
    dst.y = _origin.y + _pos.y * bmpSize;
    SDL_BlitSurface(_player, &src, _screen, &dst);
}
