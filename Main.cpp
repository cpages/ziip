#include <cassert>
#include <cstdlib>
#include <ctime>
#include "SDL_ttf.h"
#include "Main.hpp"

namespace
{
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const char* WINDOW_TITLE = "Ziip";
}

Main::Main()
{
    SDL_Init( SDL_INIT_EVERYTHING );
    assert (TTF_Init() != -1); //TODO: check ret

    _screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 0,
            SDL_HWSURFACE | SDL_DOUBLEBUF );
    SDL_WM_SetCaption( WINDOW_TITLE, 0 );

    _player.reset(new Player(_screen));
    _board.reset(new Board(_screen, _player.get()));

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
    SDL_Surface *bgdMenu = SDL_LoadBMP("main_menu.bmp");
    SDL_BlitSurface(bgdMenu, NULL, _screen, NULL);
    SDL_Flip(_screen);

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
        _player->move(lastMov);

        // draw scene
        _board->draw();
        _player->draw();

        if (cause == GameOver)
        {
            SDL_Surface *go = SDL_LoadBMP("game_over.bmp");
            SDL_Rect goRect;
            goRect.x = 350;
            goRect.y = 275;
            SDL_BlitSurface(go, NULL, _screen, &goRect);
            //SDL_FreeSurface(go);
        }

        SDL_Flip(_screen);

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

    //TODO: remove this from here
    _board->resize(WINDOW_WIDTH, WINDOW_HEIGHT);

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
                state = MainMenu;
                break;
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    Main mainApp;

    return mainApp.run();
}
