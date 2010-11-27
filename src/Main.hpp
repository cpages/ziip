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
#ifndef MAIN_HPP
#define MAIN_HPP

#include <memory>
#include <vector>
#include <string>
#include "SDL.h"
#include "SharedData.hpp"
#include "Board.hpp"
#include "Net.hpp"

class Main
{
    public:
        Main();
        ~Main();

        int run();

    private:
        enum MMenuOpts
        {
            Game1P = 0,
            Game2P,
            Options,
            Quit,
            MMNOpts
        };

        enum MPMenuOpts
        {
            GameHiScore = 0,
            GameDeathMatch,
            GameNetHS,
            GameNetDM,
            Back,
            MPMNOpts
        };

        enum State
        {
            StatMainMenu,
            StatMPMenu,
            StatAwaiting,
            StatPlay
        };

        enum GameMode
        {
            GMSinglePlayer,
            GMHiScore,
            GMDeathMatch,
            GMNetHiScore,
            GMNetDeathMatch
        };

        enum PlayExitCause
        {
            InvalidCause,
            GameOver,
            Quitted
        };

        void fillMenuOpts();
        PlayExitCause play();
        bool gameOver(const std::vector<bool> &gOver);
        int winner(const std::vector<bool> &gOver);
        bool waitForGame();
        bool isNetGame(GameMode gm);
        bool isDMGame(GameMode gm);

        std::auto_ptr<Resources> _rsc;
        std::auto_ptr<Board> _boards[4];
        std::vector<SDL_Joystick *> _joysticks;
        std::vector<std::string> _mmOpts;
        std::vector<std::string> _mpmOpts;
        GameMode _gameMode;
        int _numPlayers;
        std::auto_ptr<HiScore> _hiScore;
        Client _client;
};

#endif //MAIN_HPP
