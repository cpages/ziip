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

    _screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 0,
            SDL_HWSURFACE | SDL_DOUBLEBUF );
    SDL_WM_SetCaption( WINDOW_TITLE, 0 );

    _player.reset(new Player(_screen));
    _board.reset(new Board(_screen, _player.get()));
    _newPieces.reset(new NewPieces());
    _timer.reset(new Timer(1000, *_newPieces));
}

Main::~Main()
{
    SDL_Quit();
}

int
Main::run()
{
    SDL_Event event;
    bool gameRunning = true;
    Player::playerDirection lastMov;

    //TODO: remove this from here
    _board->resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    while (gameRunning)
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
                    case SDLK_q:
                        gameRunning = false;
                        break;
                    default:
                        //do nothing
                        break;
                }
            }
            else if (event.type == SDL_USEREVENT)
            {
                lastMov = Player::Right;
            }
            else if (event.type == SDL_QUIT)
            {
                gameRunning = false;
            }
        }

        // update state
        _player->move(lastMov);

        // draw scene
        _board->draw();
        _player->draw();

        SDL_Flip(_screen);

        SDL_Delay(1);
    }

    return 0;
}

int main(int argc, char **argv)
{
    Main mainApp;

    return mainApp.run();
}
