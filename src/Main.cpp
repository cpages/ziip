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
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include "SDL/SDL_ttf.h"
#include "Resources.hpp"
#include "Player.hpp"
#include "InputMgr.hpp"
#include "Main.hpp"

namespace
{
    const bool debug = false;

    const char* WINDOW_TITLE = "Ziip";
}

Main::Main()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD |
                SDL_INIT_JOYSTICK) == -1)
    {
        std::ostringstream msg;
        msg << "Could not initialize SDL: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    //init TTF stuff
    if (TTF_Init() == -1)
    {
        std::ostringstream msg;
        msg << "Could not initialize SDL_ttf: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    //init joystick stuff
    SDL_JoystickEventState(SDL_ENABLE);
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick *joy = SDL_JoystickOpen(i);
        _joysticks.push_back(joy);
        if (debug)
        {
            std::cout << "Joy " << i << ":" << std::endl;
            std::cout << " Axes: " << SDL_JoystickNumAxes(_joysticks[i]) <<
                std::endl;
            std::cout << " Buttons: " << SDL_JoystickNumButtons(_joysticks[i]) <<
                std::endl;
            std::cout << " Balls: " << SDL_JoystickNumBalls(_joysticks[i]) <<
                std::endl;
            std::cout << " Hats: " << SDL_JoystickNumHats(_joysticks[i]) <<
                std::endl;
        }
    }

#if 0
    SDL_Event event;
    InputMgr getKey;
    while (1)
    {
        if (SDL_WaitEvent(&event))
        {
            InputMgr::Keys key;
            if ((key = getKey(event)) != InputMgr::INV_EVT)
            {
                std::cout << key << std::endl;
                if (key == InputMgr::QUIT)
                    break;
            }
            else if (event.type == SDL_QUIT)
            {
                break;
            }
        }
    }
#endif

    SDL_WM_SetCaption( WINDOW_TITLE, 0 );

    _rsc.reset(new Resources);
    _board.reset(new Board(_rsc.get()));

    //initialize random seed
    srand(time(NULL));
}

Main::~Main()
{
    //force board destruction to release ttf related stuff before TTF_Quit call
    _board.reset();
    TTF_Quit();
    SDL_Quit();
}

Main::PlayExitCause 
Main::play()
{
    SDL_Event event;
    InputMgr getKey;
    Player::playerDirection lastMov;

    Main::PlayExitCause cause = InvalidCause;
    while (cause == InvalidCause)
    {
        // parse input
        if (SDL_WaitEvent(&event))
        {
            lastMov = Player::NoDir;
            InputMgr::Keys key;
            if ((key = getKey(event)) != InputMgr::INV_EVT)
            {
                switch (key)
                {
                    case InputMgr::UP:
                        lastMov = Player::Up;
                        break;
                    case InputMgr::DOWN:
                        lastMov = Player::Down;
                        break;
                    case InputMgr::LEFT:
                        lastMov = Player::Left;
                        break;
                    case InputMgr::RIGHT:
                        lastMov = Player::Right;
                        break;
                    case InputMgr::BUT_A:
                        _board->playerShooted();
                        break;
                    case InputMgr::QUIT:
                        cause = Quitted;
                        break;
                    default:
                        //do nothing
                        break;
                }
            }
            else if (event.type == SDL_USEREVENT)
            {
                const bool gameOver = _board->addPiece();
                if (gameOver)
                    cause = GameOver;
            }
            else if (event.type == SDL_QUIT)
            {
                cause = Quitted;
            }
        }
        else
        {
            std::ostringstream msg;
            msg << "Error while waiting for event: " << SDL_GetError();
            throw std::runtime_error(msg.str());
        }

        // update state
        _board->movePlayer(lastMov);

        // draw scene
        _board->draw();

        if (cause == GameOver)
        {
            int scrW, scrH;
            _rsc->getScreenSize(scrW, scrH);
            SDL_Rect goRect;
            // game_over is 100x40
            goRect.x = scrW / 2 - 50;
            goRect.y = scrH / 2 - 20;
            SDL_BlitSurface(_rsc->gameOver(), NULL, _rsc->screen(), &goRect);
        }

        SDL_Flip(_rsc->screen());

        SDL_Delay(1);

        if (cause == GameOver)
            SDL_Delay(5000);
    }

    return cause;
}

int
Main::run()
{
    State state = StatMainMenu;
    MainMenu mMenu(_rsc.get());
    bool quit = false;

    //TODO: change this depending on game mode
    int w, h;
    _rsc->getScreenSize(w, h);
    _board->resize(w, h);

    while (!quit)
    {
        switch (state)
        {
            case StatMainMenu:
                {
                    const MainMenu::Option mmOption = mMenu();
                    if (mmOption == MainMenu::Quit)
                        quit = true;
                    else if (mmOption == MainMenu::NewGame)
                        state = StatPlay;
                }
                break;
            case StatPlay:
                {
                    const PlayExitCause pECause = play();
                    if (pECause == Quitted)
                        quit = true;
                    else if (pECause == GameOver)
                        _board->clear();
                    state = StatMainMenu;
                }
                break;
        }
    }

    return 0;
}

const int Main::MainMenu::SelStepPx = 100;

Main::MainMenu::MainMenu(Resources *rsc):
    _rsc(rsc),
    _selOpt(0)
{
    int scrW, scrH;
    _rsc->getScreenSize(scrW, scrH);
    const int scaledSelSize = SelStepPx * _rsc->getProportion();
    SDL_Rect rect;
    rect.x = scrW / 2 - scaledSelSize/10 - scaledSelSize;
    rect.y = scrH / 2 - scaledSelSize/2;
    _selOptRect.push_back(rect);
    rect.y += scaledSelSize;
    _selOptRect.push_back(rect);
}

Main::MainMenu::Option
Main::MainMenu::operator()()
{
    SDL_Event event;
    InputMgr getKey;

    Option option = InvalidOption;
    while (option == InvalidOption)
    {
        SDL_BlitSurface(_rsc->mainMenu(), NULL, _rsc->screen(), NULL);
        SDL_BlitSurface(_rsc->mmSel(), NULL, _rsc->screen(),
                &_selOptRect[_selOpt]);
        SDL_Flip(_rsc->screen());

        if (SDL_WaitEvent(&event))
        {
            InputMgr::Keys key;
            if ((key = getKey(event)) != InputMgr::INV_EVT)
            {
                switch (key)
                {
                    case InputMgr::UP:
                        _selOpt = 0;
                        break;
                    case InputMgr::DOWN:
                        _selOpt = 1;
                        break;
                    case InputMgr::BUT_A:
                        option = static_cast<Option>(_selOpt);
                    default:
                        break;
                }
            }
            else if (event.type == SDL_QUIT)
            {
                option = Quit;
            }
        }
    }

    return option;
}

int main(int argc, char *argv[])
{
    int ret;

    try
    {
        Main mainApp;

        ret = mainApp.run();
    }
    catch (std::runtime_error &rte)
    {
        std::cerr << rte.what() << std::endl;
        ret = -1;
    }

    return ret;
}