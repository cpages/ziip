#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "SharedData.hpp"

class Player
{
    public:
        enum playerDirection
        {
            Up = 0,
            Down,
            Left,
            Right,
            NoDir
        };

        Player(SDL_Surface *screen);
        ~Player();

        void setOriginAndSize(SDL_Rect rect);
        void move(playerDirection mov);
        void draw();

    private:
        struct playerPos
        {
            int x;
            int y;
        };

        SDL_Rect _rect;
        playerPos _pos;
        int _color;
        playerDirection _dir;
        SDL_Surface *_player;
        SDL_Surface *_screen;
};

#endif //PLAYER_HPP
