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

    _newPieces.reset(new NewPieces());
    _timer.reset(new Timer(1000, *_newPieces));
}

Main::~Main()
{
    SDL_FreeSurface(_bitmap);

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
    _bitmap = SDL_LoadBMP("bat.bmp");

    // Part of the bitmap that we want to draw
    SDL_Rect source;
    source.x = 24;
    source.y = 63;
    source.w = 65;
    source.h = 44;

    // Part of the screen we want to draw the sprite to
    SDL_Rect destination;
    destination.x = 100;
    destination.y = 100;
    destination.w = 65;
    destination.h = 44;

    SDL_Event event;
    bool gameRunning = true;

    while (gameRunning)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameRunning = false;
            }
        }

        destination.x += getNumNewPieces() * 10;

        SDL_BlitSurface(_bitmap, &source, _screen, &destination);

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

/*int main(int argc, char **argv)
{
    SDL_Init( SDL_INIT_VIDEO );

    SDL_Surface* screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 0,
            SDL_HWSURFACE | SDL_DOUBLEBUF );
    SDL_WM_SetCaption( WINDOW_TITLE, 0 );

    SDL_Rect boardRect;
    boardRect.x = 0;
    boardRect.y = 0;
    boardRect.w = 756;
    boardRect.h = 588;
    Board::Board(SDL_Rect size)

    SDL_Surface* bitmap = SDL_LoadBMP("bat.bmp");

    // Part of the bitmap that we want to draw
    SDL_Rect source;
    source.x = 24;
    source.y = 63;
    source.w = 65;
    source.h = 44;

    // Part of the screen we want to draw the sprite to
    SDL_Rect destination;
    destination.x = 100;
    destination.y = 100;
    destination.w = 65;
    destination.h = 44;

    SDL_Event event;
    bool gameRunning = true;

    while (gameRunning)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameRunning = false;
            }
        }

        SDL_BlitSurface(bitmap, &source, screen, &destination);

        SDL_Flip(screen);

        SDL_Delay(1);
    }

    SDL_FreeSurface(bitmap);

    SDL_Quit();

    return 0;
}*/
