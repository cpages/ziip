#ifndef MAIN_HPP
#define MAIN_HPP

#include <memory>
#include "SDL.h"
#include "SharedData.hpp"
#include "Board.hpp"

class Main
{
    public:
        Main();
        ~Main();

        int run();

    private:
        class MainMenu
        {
            public:
                enum Option
                {
                    NewGame = 0,
                    Quit,
                    InvalidOption
                };

                MainMenu(Resources *rsc);

                Option operator()();

            private:
                static const int SelStepPx;
                Resources *_rsc;
                size_t _selOpt;
                std::vector<SDL_Rect> _selOptRect;
        };

        enum State
        {
            StatMainMenu,
            StatPlay
        };

        enum PlayExitCause
        {
            InvalidCause,
            GameOver,
            Quitted
        };

        PlayExitCause play();

        std::auto_ptr<Resources> _rsc;
        std::auto_ptr<Board> _board;
};

#endif //MAIN_HPP
