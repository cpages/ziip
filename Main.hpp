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
        enum State
        {
            MainMenu,
            Play
        };
        enum MainMenuOption
        {
            InvalidOption,
            NewGame,
            Quit
        };
        enum PlayExitCause
        {
            InvalidCause,
            GameOver,
            Quitted
        };

        MainMenuOption mainMenu();
        PlayExitCause play();

        std::auto_ptr<Resources> _rsc;
        std::auto_ptr<Board> _board;
};

#endif //MAIN_HPP
