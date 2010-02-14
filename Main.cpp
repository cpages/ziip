#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include "SDL_ttf.h"
#include "Resources.hpp"
#include "Player.hpp"
#include "Main.hpp"

namespace
{
    const char* WINDOW_TITLE = "Ziip";
}

Main::Main()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD) == -1)
    {
        std::ostringstream msg;
        msg << "Could not initialize SDL: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    if (TTF_Init() == -1)
    {
        std::ostringstream msg;
        msg << "Could not initialize SDL_ttf: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    SDL_WM_SetCaption( WINDOW_TITLE, 0 );

    _rsc.reset(new Resources);
    _board.reset(new Board(_rsc.get()));

    //initialize random seed
    srand(time(NULL));
}

Main::~Main()
{
    TTF_Quit();
    SDL_Quit();
}

Main::MainMenuOption
Main::mainMenu()
{
    SDL_BlitSurface(_rsc->mainMenu(), NULL, _rsc->screen(), NULL);
    SDL_Flip(_rsc->screen());

    Main::MainMenuOption option = InvalidOption;
    SDL_Event event;
    while (option == InvalidOption)
    {
        if (SDL_WaitEvent(&event))
        {
            if( event.type == SDL_KEYDOWN )
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        option = Quit;
                        break;
                    case SDLK_s:
                        option = NewGame;
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

Main::PlayExitCause 
Main::play()
{
    SDL_Event event;
    Player::playerDirection lastMov;

    Main::PlayExitCause cause = InvalidCause;
    while (cause == InvalidCause)
    {
        // parse input
        if (SDL_WaitEvent(&event))
        {
            lastMov = Player::NoDir;
            if( event.type == SDL_KEYDOWN )
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                        lastMov = Player::Up;
                        break;
                    case SDLK_DOWN:
                        lastMov = Player::Down;
                        break;
                    case SDLK_LEFT:
                        lastMov = Player::Left;
                        break;
                    case SDLK_RIGHT:
                        lastMov = Player::Right;
                        break;
                    case SDLK_SPACE:
                        _board->playerShooted();
                        break;
                    case SDLK_q:
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
    State state = MainMenu;
    bool quit = false;

    //TODO: change this depending on game mode
    int w, h;
    _rsc->getScreenSize(w, h);
    _board->resize(w, h);

    while (!quit)
    {
        switch (state)
        {
            Main::MainMenuOption mmOption;
            Main::PlayExitCause pECause;
            case MainMenu:
                mmOption = mainMenu();
                if (mmOption == Quit)
                    quit = true;
                else if (mmOption == NewGame)
                    state = Play;
                break;
            case Play:
                pECause = play();
                if (pECause == Quitted)
                    quit = true;
                else if (pECause == GameOver)
                    _board->clear();
                state = MainMenu;
                break;
        }
    }

    return 0;
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
