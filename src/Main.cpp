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
#include "Resources.hpp"
#include "Player.hpp"
#include "InputMgr.hpp"
#include "HiScore.hpp"
#include "Main.hpp"

namespace
{
    const bool debug = false;

    const char* WINDOW_TITLE = "Ziip";

}

Main::Main():
    _numPlayers(0)
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD |
                SDL_INIT_JOYSTICK) == -1)
    {
        std::ostringstream msg;
        msg << "Could not initialize SDL: " << SDL_GetError();
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

    SDL_WM_SetCaption( WINDOW_TITLE, 0 );

    _rsc.reset(new Resources);
    _hiScore.reset(new HiScore(_rsc.get()));

    //initialize random seed
    srand(time(NULL));
}

Main::~Main()
{
    SDL_Quit();
}

Main::PlayExitCause 
Main::play()
{
    SDL_Event event;
    InputMgr getKey;
    Player::playerDirection lastMov = Player::NoDir;
    std::vector<bool> gOver(_numPlayers, false);
    int id = 0;

    for (int i = 0; i < _numPlayers; ++i)
        _boards[i]->draw();

    Main::PlayExitCause cause = InvalidCause;
    while (cause == InvalidCause)
    {
        // parse input
        if (SDL_WaitEvent(&event))
        {
            lastMov = Player::NoDir;
            InputMgr::KeyPressed keyPress;
            keyPress = getKey(event);
            if (keyPress.key != InputMgr::INV_EVT)
            {
                id = keyPress.playerId;
                switch (keyPress.key)
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
                        _boards[id]->playerShooted();
                        break;
                    case InputMgr::QUIT:
                        cause = Quitted;
                        break;
                    default:
                        //do nothing
                        break;
                }
            }
            else if (event.type == EVT_TIMER)
            {
                id = event.user.code;
                gOver[id] = _boards[id]->addPiece();
                //TODO: do we need to go through Main?
                if (gOver[id])
                    _boards[id]->gameOver();
                if (gameOver(gOver))
                {
                    _hiScore->addScore(_boards[id]->getScore());
                    cause = GameOver;
                }
            }
            else if (event.type == EVT_PIECE && _gameMode == GMDeathMatch)
            {
                id = event.user.code;
                int &numPieces = *static_cast<int *>(event.user.data1);
                for (int i = 0; i < _numPlayers; ++i)
                {
                    if (id == i)
                        continue;
                    for (int j = 0; j < numPieces; ++j)
                    {
                        gOver[i] = _boards[i]->addPiece();
                        //TODO: do we need to go through Main?
                        if (gOver[i])
                        {
                            _boards[i]->gameOver();
                            break;
                        }
                    }
                }
                numPieces = 0;
                if (gameOver(gOver))
                {
                    //_hiScore->addScore(_boards[i]->getScore());
                    cause = GameOver;
                }
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
        if (lastMov != Player::NoDir)
            _boards[id]->movePlayer(lastMov);

        // draw scene
        if (_gameMode == GMDeathMatch) //paint all, because of incoming pieces
            for (int i = 0; i < _numPlayers; ++i)
                _boards[i]->draw();
        else
            _boards[id]->draw();

        if (cause == GameOver)
        {
            const std::string goStr("Game Over");
            const SDL_Color goCol = {255, 255, 255};
            SDL_Surface *go = _rsc->renderText(goStr, goCol);
            int scrW, scrH;
            _rsc->getScreenSize(scrW, scrH);
            SDL_Rect goRect;
            goRect.x = scrW / 2 - 60;
            goRect.y = scrH / 2 - 12;
            SDL_BlitSurface(go, NULL, _rsc->screen(), &goRect);
            SDL_FreeSurface(go);
        }

        SDL_Flip(_rsc->screen());

        SDL_Delay(1);

        if (cause == GameOver)
        {
            SDL_Delay(2000);
            while (SDL_PollEvent(&event)); //empty evt queue
            InputMgr::KeyPressed keyPress;
            keyPress.key = InputMgr::INV_EVT;
            while (keyPress.key == InputMgr::INV_EVT)
            {
                //TODO: check for exit
                if (SDL_WaitEvent(&event))
                    keyPress = getKey(event);
            }
            SDL_Surface *hsSfc = _hiScore->renderHiScore();
            SDL_BlitSurface(hsSfc, NULL, _rsc->screen(), NULL);
            SDL_FreeSurface(hsSfc);
            SDL_Flip(_rsc->screen());
            SDL_Delay(1);
            keyPress.key = InputMgr::INV_EVT;
            while (keyPress.key == InputMgr::INV_EVT)
            {
                //TODO: check for exit
                if (SDL_WaitEvent(&event))
                    keyPress = getKey(event);
            }
        }
    }

    return cause;
}

bool
Main::gameOver(const std::vector<bool> &gOver)
{
    switch (_gameMode)
    {
        case Main::GMSinglePlayer:
            return gOver[0];
            break;
        case Main::GMHiScore:
            for (unsigned int i = 0; i < gOver.size(); ++i)
                if (!gOver[i])
                    return false;
            return true;
            break;
        case Main::GMDeathMatch:
            {
                int pAlive = 0;
                for (unsigned int i = 0; i < gOver.size(); ++i)
                    if (!gOver[i])
                        pAlive++;
                assert (pAlive != 0);
                if (pAlive == 1)
                    return true;
                return false;
            }
            break;
    }
    // make g++ happy
    assert (0);
    return false;
}

int
Main::run()
{
    State state = StatMainMenu;
    MainMenu mMenu(_rsc.get());
    MPMenu mpMenu(_rsc.get());
    bool quit = false;

    while (!quit)
    {
        switch (state)
        {
            case StatMainMenu:
                {
                    const MainMenu::Option mmOption = mMenu();
                    if (mmOption == MainMenu::Quit)
                        quit = true;
                    else if (mmOption == MainMenu::Game1P)
                    {
                        _gameMode = GMSinglePlayer;
                        _numPlayers = 1;
                        state = StatPlay;
                    }
                    else if (mmOption == MainMenu::Game2P)
                    {
                        state = StatMPMenu;
                    }
                    else if (mmOption == MainMenu::Options)
                    {
                        //do nothing
                    }
                }
                break;
            case StatMPMenu:
                {
                    const MPMenu::Option mpmOption = mpMenu();
                    if (mpmOption == MPMenu::Quit)
                        quit = true;
                    else if (mpmOption == MPMenu::GameHiScore)
                    {
                        _gameMode = GMHiScore;
                        _numPlayers = 2;
                        state = StatPlay;
                    }
                    else if (mpmOption == MPMenu::GameDeathMatch)
                    {
                        _gameMode = GMDeathMatch;
                        _numPlayers = 2;
                        state = StatPlay;
                    }
                    else if (mpmOption == MPMenu::Back)
                    {
                        state = StatMainMenu;
                    }
                }
                break;
            case StatPlay:
                {
                    _rsc->prepareBoardGraphics(_numPlayers);
                    for (int i = 0; i < _numPlayers; ++i)
                        _boards[i].reset(new Board(i, _rsc.get()));
                    for (int i = _numPlayers; i < 4; ++i)
                        _boards[i].reset();
                    const PlayExitCause pECause = play();
                    if (pECause == Quitted)
                        quit = true;
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
    const int scaledSelSize = SelStepPx * _rsc->getBgScale();
    SDL_Rect rect;
    rect.x = scrW / 2 - scaledSelSize;
    rect.y = scrH / 2 - scaledSelSize/2 - scaledSelSize;
    for (int i = 0; i < MainMenu::NumOptions; ++i)
    {
        _selOptRect.push_back(rect);
        rect.y += scaledSelSize;
    }
}

Main::MainMenu::Option
Main::MainMenu::operator()()
{
    SDL_Event event;
    InputMgr getKey;

    Option option = InvalidOption;
    while (option == InvalidOption)
    {
        SDL_BlitSurface(_rsc->getSfc(Resources::SfcMainMenu), NULL,
                _rsc->screen(), NULL);
        SDL_BlitSurface(_rsc->getSfc(Resources::SfcMMSel), NULL,
                _rsc->screen(), &_selOptRect[_selOpt]);
        SDL_Flip(_rsc->screen());

        if (SDL_WaitEvent(&event))
        {
            InputMgr::KeyPressed keyPress;
            keyPress = getKey(event);
            if (keyPress.key != InputMgr::INV_EVT)
            {
                switch (keyPress.key)
                {
                    case InputMgr::UP:
                        if (_selOpt > 0)
                            _selOpt--;
                        break;
                    case InputMgr::DOWN:
                        if (_selOpt < MainMenu::NumOptions - 1)
                            _selOpt++;
                        break;
                    case InputMgr::BUT_A:
                        option = static_cast<Option>(_selOpt);
                        break;
                    case InputMgr::QUIT:
                        option = Quit;
                        break;
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

const int Main::MPMenu::SelStepPx = 100;

Main::MPMenu::MPMenu(Resources *rsc):
    _rsc(rsc),
    _selOpt(0)
{
    int scrW, scrH;
    _rsc->getScreenSize(scrW, scrH);
    const int scaledSelSize = SelStepPx * _rsc->getBgScale();
    SDL_Rect rect;
    rect.x = scrW / 2 - scaledSelSize;
    rect.y = scrH / 2 - scaledSelSize/2;
    for (int i = 0; i < MPMenu::NumOptions; ++i)
    {
        _selOptRect.push_back(rect);
        rect.y += scaledSelSize;
    }
}

Main::MPMenu::Option
Main::MPMenu::operator()()
{
    SDL_Event event;
    InputMgr getKey;

    Option option = InvalidOption;
    while (option == InvalidOption)
    {
        SDL_BlitSurface(_rsc->getSfc(Resources::SfcMPMenu), NULL,
                _rsc->screen(), NULL);
        SDL_BlitSurface(_rsc->getSfc(Resources::SfcMMSel), NULL,
                _rsc->screen(), &_selOptRect[_selOpt]);
        SDL_Flip(_rsc->screen());

        if (SDL_WaitEvent(&event))
        {
            InputMgr::KeyPressed keyPress;
            keyPress = getKey(event);
            if (keyPress.key != InputMgr::INV_EVT)
            {
                switch (keyPress.key)
                {
                    case InputMgr::UP:
                        if (_selOpt > 0)
                            _selOpt--;
                        break;
                    case InputMgr::DOWN:
                        if (_selOpt < MPMenu::NumOptions - 1)
                            _selOpt++;
                        break;
                    case InputMgr::BUT_A:
                        option = static_cast<Option>(_selOpt);
                        break;
                    case InputMgr::QUIT:
                        option = Quit;
                        break;
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
