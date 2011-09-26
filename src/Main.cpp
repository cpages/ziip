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
#ifdef GEKKO
#include <debug.h>
#endif
#include "Resources.hpp"
#include "Player.hpp"
#include "InputMgr.hpp"
#include "HiScore.hpp"
#include "Menu.hpp"
#include "LuaIface.hpp"
#include "Main.hpp"

namespace
{
    const bool debug = false;

    const char* WINDOW_TITLE = "Ziip";

    void
    printUsage()
    {
        //TODO: add (c) notice and version
        std::cout << "Usage: ziip [s]" << std::endl;
    }
}

Main::Main():
    _numPlayers(0)
{
    //SDL_INIT_EVENTTHREAD is not supported under win, commenting it out
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | //SDL_INIT_EVENTTHREAD |
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

    fillMenuOpts();
}

Main::~Main()
{
    SDL_Quit();
}

void
Main::fillMenuOpts()
{
    _mmOpts.resize(MMNOpts);
    _mmOpts[Game1P] = std::string("Play");
    _mmOpts[Game2P] = std::string("Multiplayer");
    //_mmOpts[Options] = std::string("Options");
    _mmOpts[Quit] = std::string("Quit");

    _mpmOpts.resize(MPMNOpts);
    _mpmOpts[GameHiScore] = std::string("Highscore");
    _mpmOpts[GameDeathMatch] = std::string("Deathmatch");
    _mpmOpts[GameNetHS] = std::string("Netplay Highscore");
    _mpmOpts[GameNetDM] = std::string("Netplay Deathmatch");
    _mpmOpts[Back] = std::string("Back");
}

Main::PlayExitCause 
Main::play()
{
    SDL_Event event;
    std::vector<bool> gOver(_numPlayers, false);
    Player::playerDirection lastMov = Player::NoDir;
    bool sendState = false; // for net play
    int id = -1;
    // for net deathmatch
    std::vector<int> rcvdPieces(_numPlayers, 0);
    int pendPieces = 0; // for net deatchmatch
    bool repaint = false;

    bool lua = false;
    LuaIface test;
    if (_boards[1].get())
    {
        test.createBot(1, _boards[1].get(), "basicBot.lua");
        //test.setBoard(_boards[1].get());
        lua = true;
    }

    // initial paint
    for (int i = 0; i < _numPlayers; ++i)
        _boards[i]->draw();
    SDL_Flip(_rsc->screen());
    SDL_Delay(1);

    Main::PlayExitCause cause = InvalidCause;
    while (cause == InvalidCause)
    {
        // parse input
        if (SDL_WaitEvent(&event))
        {
            lastMov = Player::NoDir;
            sendState = false;
            id = -1;
            repaint = false;

            InputMgr::KeyPressed keyPress;
            keyPress = _rsc->getKey(event);
            if (keyPress.key != InputMgr::INV_EVT)
            {
                id = keyPress.playerId;
                if (id >= _numPlayers)
                    continue;
                repaint = true;
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
                        sendState = true;
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
                repaint = true;
                id = event.user.code;
                if (isNetGame(_gameMode) && id != 0)
                    continue;
                assert (id < _numPlayers);
                gOver[id] = _boards[id]->addPiece();
                //TODO: do we need to go through Main?
                if (gOver[id])
                    _boards[id]->gameOver();
                if (gameOver(gOver))
                {
                    _hiScore->addScore(_boards[id]->getScore());
                    cause = GameOver;
                }
                sendState = true;
            }
            else if (event.type == EVT_PIECE && _gameMode == GMDeathMatch)
            {
                repaint = true;
                id = event.user.code;
                assert (id < _numPlayers);
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
            else if (event.type == EVT_CHECK_NET)
            {
                while (_client.listen(0));
                Board::State state;
                if (_client.newState(state))
                {
                    id = 1;
                    _boards[id]->setState(state);
                    if (_gameMode == GMNetDeathMatch)
                    {
                        pendPieces += state.sentPieces - rcvdPieces[id];
                        rcvdPieces[id] = state.sentPieces;
                        for (int j = 0; j < pendPieces; ++j)
                        {
                            gOver[0] = _boards[0]->addPiece();
                        }
                        pendPieces = 0;
                    }
                    if (state.gameOver)
                    {
                        gOver[id] = true;
                        if (gameOver(gOver))
                            cause = GameOver;
                    }
                    repaint = true;
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

        if (id == -1) //unhandled event, don't do nothing
            continue;
        assert (id != -1);

        // update state
        if (lastMov != Player::NoDir)
        {
            _boards[id]->movePlayer(lastMov);
            if (lua)
                test.move(1);
            sendState = true;
        }

        // update remote players
        if (isNetGame(_gameMode))
        {
            _client.sendState(_boards[0]->getState());
        }

        // draw scene
        if (repaint)
        {
            if (isDMGame(_gameMode)) //paint all, because of incoming pieces
                for (int i = 0; i < _numPlayers; ++i)
                    _boards[i]->draw();
            else
            {
                _boards[id]->draw();
                _boards[1]->draw();
            }
        }

        if (cause == GameOver)
        {
            std::ostringstream sstr;
            switch (_gameMode)
            {
                case GMSinglePlayer:
                    sstr << "Game Over";
                    break;
                case GMHiScore:
                case GMNetHiScore:
                    {
                        const int w = winner(gOver);
                        if (w)
                            sstr << "Player " << w << " wins!";
                        else
                            sstr << "Draw";
                    }
                    break;
                case GMDeathMatch:
                    sstr << "Player " << winner(gOver) << " wins!";
                    break;
                case GMNetDeathMatch:
                    if (winner(gOver) == 1)
                        sstr << "You win!";
                    else
                        sstr << "You lose!";
                    break;
            }
            const std::string goStr(sstr.str());
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
                //TODO: error check
                SDL_WaitEvent(&event);
                keyPress = _rsc->getKey(event);
                if (event.type == SDL_QUIT ||
                        keyPress.key == InputMgr::QUIT)
                {
                    cause = Quitted;
                    break;
                }
            }
            // don't show hiscore in deathmatch or if user has quit
            if (!isDMGame(_gameMode) &&
                    cause != Quitted)
            {
                SDL_Surface *hsSfc = _hiScore->renderHiScore();
                SDL_BlitSurface(hsSfc, NULL, _rsc->screen(), NULL);
                SDL_FreeSurface(hsSfc);
                SDL_Flip(_rsc->screen());
                SDL_Delay(1);
                keyPress.key = InputMgr::INV_EVT;
                while (keyPress.key == InputMgr::INV_EVT)
                {
                    //TODO: error check
                    SDL_WaitEvent(&event);
                    keyPress = _rsc->getKey(event);
                    if (event.type == SDL_QUIT ||
                            keyPress.key == InputMgr::QUIT)
                    {
                        cause = Quitted;
                        break;
                    }
                }
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
        case GMSinglePlayer:
            return gOver[0];
            break;
        case GMHiScore:
        case GMNetHiScore:
            for (unsigned int i = 0; i < gOver.size(); ++i)
                if (!gOver[i])
                    return false;
            return true;
            break;
        case GMDeathMatch:
        case GMNetDeathMatch:
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
Main::winner(const std::vector<bool> &gOver)
{
    int winPlayer = -1;
    switch (_gameMode)
    {
        case (GMSinglePlayer):
            assert (0); //we shouldn't be calling this
            break;
        case GMHiScore:
        case GMNetHiScore:
            {
                winPlayer = 0; //draw by default
                int hiScore = 0;
                for (int i = 0; i < _numPlayers; ++i)
                {
                    const int score = _boards[i]->getScore();
                    if (score >= hiScore) 
                    {
                        if (score > hiScore)
                            winPlayer = i+1;
                        else
                            winPlayer = 0; //draw
                        hiScore = score;
                    }
                }
            }
            break;
        case GMDeathMatch:
        case GMNetDeathMatch:
            for (int i = 0; i < _numPlayers; ++i)
            {
                if (gOver[i] == false)
                {
                    winPlayer = i+1;
                    break;
                }
            }
            break;
    }

    assert (winPlayer != -1);

    return winPlayer;
}

bool
Main::waitForGame()
{
    SDL_Event event;
    bool go = false;
    while (!go)
    {
        if (SDL_WaitEvent(&event))
        {
            InputMgr::KeyPressed keyPress;
            keyPress = _rsc->getKey(event);
            if (keyPress.key == InputMgr::QUIT ||
                    event.type == SDL_QUIT)
            {
                return false;
            }
            else if (event.type == EVT_CHECK_NET)
            {
                _client.listen(0);
                go = _client.startGame();
            }
        }
    }
    return go;
}

bool
Main::isNetGame(GameMode gm)
{
    if (gm == GMNetHiScore ||
            gm == GMNetDeathMatch)
        return true;
    return false;
}

bool
Main::isDMGame(GameMode gm)
{
    if (gm == GMDeathMatch ||
            gm == GMNetDeathMatch)
        return true;
    return false;
}

int
Main::run()
{
    //dummy title, for now
    const std::string dt("har");
    State state = StatMainMenu;
    bool quit = false;

    while (!quit)
    {
        switch (state)
        {
            case StatMainMenu:
                {
                    Menu mMenu(_rsc.get(), dt, _mmOpts);
                    const int mmOption = mMenu();
                    if (mmOption == -2 || mmOption == Quit)
                        quit = true;
                    else if (mmOption == Game1P)
                    {
                        _gameMode = GMSinglePlayer;
                        _numPlayers = 1;
                        state = StatPlay;
                    }
                    else if (mmOption == Game2P)
                    {
                        state = StatMPMenu;
                    }
                    /*
                    else if (mmOption == Options)
                    {
                        //do nothing
                    }
                    */
                }
                break;
            case StatMPMenu:
                {
                    Menu mpMenu(_rsc.get(), dt, _mpmOpts);
                    const int mpmOption = mpMenu();
                    if (mpmOption == -2)
                        quit = true;
                    else if (mpmOption == GameHiScore)
                    {
                        _gameMode = GMHiScore;
                        _numPlayers = 2;
                        state = StatPlay;
                    }
                    else if (mpmOption == GameDeathMatch)
                    {
                        _gameMode = GMDeathMatch;
                        _numPlayers = 2;
                        state = StatPlay;
                    }
                    else if (mpmOption == GameNetHS)
                    {
                        _client.newGame();
                        _gameMode = GMNetHiScore;
                        _numPlayers = 2;
                        if (_client.connect())
                        {
                            state = StatAwaiting;
                            std::cout << "Awaiting..." << std::endl;
                        }
                    }
                    else if (mpmOption == GameNetDM)
                    {
                        _client.newGame();
                        _gameMode = GMNetDeathMatch;
                        _numPlayers = 2;
                        if (_client.connect())
                        {
                            state = StatAwaiting;
                            std::cout << "Awaiting..." << std::endl;
                        }
                    }
                    else if (mpmOption == Back)
                    {
                        state = StatMainMenu;
                    }
                }
                break;
            case StatAwaiting:
                if (waitForGame())
                    state = StatPlay;
                else
                    state = StatMainMenu;
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

int main(int argc, char *argv[])
{
    int ret = -1;

#ifdef GEKKO
    //DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
#endif

    if (argc > 2)
    {
        printUsage();
    }
    else if (argc == 2)
    {
        const std::string param(argv[1]);
        if (!param.compare("s"))
        {
            std::cout << "Starting in server mode..." << std::endl;
            Server srv;
            srv.listen();
            ret = 0;
        }
        else
        {
            std::cout << "Wrong parameter" << std::endl;
            printUsage();
        }
    }
    else
    {
        try
        {
            Main mainApp;
            ret = mainApp.run();
        }
        catch (std::runtime_error &rte)
        {
            std::cerr << rte.what() << std::endl;
        }
    }

    return ret;
}
