#ifndef MAIN_HPP
#define MAIN_HPP

#include <memory>
#include "SDL.h"
#include "SharedData.hpp"
#include "Timer.hpp"
#include "Board.hpp"
#include "Player.hpp"

class Main
{
    public:
        Main();
        ~Main();

        int run();

    private:
        int getNumNewPieces();

        std::auto_ptr<NewPieces> _newPieces;
        std::auto_ptr<Timer> _timer;
        std::auto_ptr<Board> _board;
        std::auto_ptr<Player> _player;
        SDL_Surface *_screen;
        SDL_Surface *_bitmap;
};

#endif //MAIN_HPP
