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

        struct playerPos
        {
            int x;
            int y;
        };

        Player(Resources *rsc);
        ~Player();

        void reset();
        void setOriginAndSize(SDL_Rect rect);
        playerPos getPos() const;
        playerDirection getDirection() const;
        void reverse();
        colors getColor() const;
        void setColor(colors newColor);
        void move(playerDirection mov);
        void draw();

    private:
        Resources *_rsc;
        SDL_Rect _rect;
        playerPos _pos;
        colors _color;
        playerDirection _dir;
};

#endif //PLAYER_HPP
