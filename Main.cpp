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

    _board.reset(new Board(_screen));
    _player.reset(new Player(_screen));
    _newPieces.reset(new NewPieces());
    _timer.reset(new Timer(1000, *_newPieces));
}

Main::~Main()
{
    SDL_Quit();
}

int
Main::getNumNewPieces()
{
    int ret;

    while (SDL_mutexP(_newPieces->_mutex) != 0);
    ret = _newPieces->_num;
    _newPieces->_num = 0;
    SDL_mutexV(_newPieces->_mutex);

    return ret;
}

int
Main::run()
{
    SDL_Event event;
    bool gameRunning = true;

    _player->setOrigin(316, 216);
    Player::playerDirection lastMov;

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
                    default:
                        //do nothing
                        break;
                }
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
