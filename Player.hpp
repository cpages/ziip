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

        void setOrigin(int x, int y);
        void move(playerDirection mov);
        void draw();

    private:
        struct playerPos
        {
            int x;
            int y;
        };

        coord _origin;
        playerPos _pos;
        int _color;
        playerDirection _dir;
        SDL_Rect _rect;
        SDL_Surface *_player;
        SDL_Surface *_screen;
};

#endif //PLAYER_HPP
